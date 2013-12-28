/*
 * assembly_restore.c
 *
 *  Created on: Nov 27, 2013
 *      Author: fumin
 */
#include "destor.h"
#include "jcr.h"
#include "recipe/recipestore.h"
#include "storage/containerstore.h"
#include "restore.h"

struct {
	GQueue *area;
	int64_t area_size;
	int64_t size;
} assembly_area;

static void init_assembly_area() {
	assembly_area.area = g_queue_new();
	assembly_area.size = 0;
	assembly_area.area_size = (destor.restore_cache[1] - 1) * CONTAINER_SIZE;
}

/*
 * Forward assembly.
 * Return a queue of assembled chunks.
 * Return NULL if area is empty.
 */
static GQueue* assemble_area() {

	if (g_queue_get_length(assembly_area.area) == 0)
		return NULL;

	GQueue *q = g_queue_new();

	struct chunk *c = NULL;
	while (g_queue_get_length(assembly_area.area) > 0) {
		c = g_queue_peek_head(assembly_area.area);
		if (CHECK_CHUNK(c, CHUNK_FILE_START) || CHECK_CHUNK(c, CHUNK_FILE_END)) {
			c = g_queue_pop_head(assembly_area.area);
			g_queue_push_tail(q, c);
			c = NULL;
		} else {
			break;
		}
	}

	if (!c)
		return q;

	containerid id = c->id;

	struct container *con = NULL;
	jcr.read_container_num++;
	if (destor.simulation_level == SIMULATION_NO)
		con = retrieve_container_by_id(id);

	int i, n = g_queue_get_length(assembly_area.area);
	for (i = 0; i < n; i++) {
		struct chunk *c = g_queue_peek_nth(assembly_area.area, i);
		if (!CHECK_CHUNK(c, CHUNK_FILE_START) && !CHECK_CHUNK(c, CHUNK_FILE_END)
				&& id == c->id) {
			if (destor.simulation_level == SIMULATION_NO) {
				struct chunk *buf = get_chunk_in_container(con, &c->fp);
				assert(c->size == buf->size);
				c->data = malloc(c->size);
				memcpy(c->data, buf->data, c->size);
				free_chunk(buf);
			}
			assembly_area.size -= c->size;
			SET_CHUNK(c, CHUNK_READY);
		}
	}

	while (g_queue_get_length(assembly_area.area) > 0) {
		struct chunk *rc = g_queue_peek_head(assembly_area.area);
		if (CHECK_CHUNK(rc,
				CHUNK_FILE_START) || CHECK_CHUNK(rc, CHUNK_FILE_END) || CHECK_CHUNK(rc, CHUNK_READY)) {
			g_queue_pop_head(assembly_area.area);
			g_queue_push_tail(q, rc);
		} else {
			break;
		}
	}
	return q;
}

static int assembly_area_push(struct chunk* c) {
	/* Indicates end */
	if (c == NULL)
		return 1;

	g_queue_push_tail(assembly_area.area, c);

	if (CHECK_CHUNK(c, CHUNK_FILE_START) || CHECK_CHUNK(c, CHUNK_FILE_END))
		return 0;

	assembly_area.size += c->size;

	if (assembly_area.size >= assembly_area.area_size)
		return 1;

	return 0;
}

void* assembly_restore_thread(void *arg) {
	init_assembly_area();

	struct chunk* c;
	while ((c = sync_queue_pop(restore_recipe_queue))) {

		TIMER_DECLARE(1);
		TIMER_BEGIN(1);

		if (assembly_area_push(c)) {
			/* Full */
			GQueue *q = assemble_area();

			TIMER_END(1, jcr.read_chunk_time);

			struct chunk* rc;
			while ((rc = g_queue_pop_head(q)))
				sync_queue_push(restore_chunk_queue, rc);

			g_queue_free(q);
		} else {
			TIMER_END(1, jcr.read_chunk_time);
		}

	}

	assembly_area_push(NULL);

	GQueue *q;
	TIMER_DECLARE(1);
	TIMER_BEGIN(1);
	while ((q = assemble_area())) {
		TIMER_END(1, jcr.read_chunk_time);
		struct chunk* rc;
		while ((rc = g_queue_pop_head(q)))
			sync_queue_push(restore_chunk_queue, rc);
		TIMER_BEGIN(1);
		g_queue_free(q);
	}

	sync_queue_term(restore_chunk_queue);
	return NULL;
}