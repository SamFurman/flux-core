#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <flux/core.h>

#include "src/common/libutil/xzmalloc.h"
#include "src/common/libutil/log.h"

typedef struct {
    flux_t h;
} ctx_t;

static void freectx (ctx_t *ctx)
{
    free (ctx);
}

static ctx_t *getctx (flux_t h)
{
    ctx_t *ctx = (ctx_t *)flux_aux_get (h, "req");

    if (!ctx) {
        ctx = xzmalloc (sizeof (*ctx));
        ctx->h = h;
        flux_aux_set (h, "req", ctx, (FluxFreeFn)freectx);
    }
    return ctx;
}

/* Handle the simplest possible request.
 * Verify that everything is as expected; log it and stop the reactor if not.
 */
static int null_request_cb (flux_t h, int typemask, zmsg_t **zmsg, void *arg)
{
    //ctx_t *ctx = arg;
    char *topic = NULL;
    int type, size, flags;
    int rc = -1;
    void *buf;

    if (!zmsg || !*zmsg) {
        flux_log (h, LOG_ERR, "%s: got NULL zmsg!", __FUNCTION__);
        goto done;
    }
    if (flux_msg_get_type (*zmsg, &type) < 0) {
        flux_log (h, LOG_ERR, "%s: flux_msg_get_type: %s", __FUNCTION__,
                  strerror (errno));
        goto done;
    }
    if (type != FLUX_MSGTYPE_REQUEST) {
        flux_log (h, LOG_ERR, "%s: unexpected type %s", __FUNCTION__,
                  flux_msgtype_string (type));
        goto done;
    }
    if (flux_msg_get_topic (*zmsg, &topic) < 0) {
        flux_log (h, LOG_ERR, "%s: flux_msg_get_topic: %s", __FUNCTION__,
                  strerror (errno));
        goto done;
    }
    if (strcmp (topic, "req.null") != 0) {
        flux_log (h, LOG_ERR, "%s: unexpected topic: %s", __FUNCTION__,
                  topic);
        goto done;
    }
    if (flux_msg_get_payload (*zmsg, &flags, &buf, &size) == 0) {
        flux_log (h, LOG_ERR, "%s: unexpected payload size %d", __FUNCTION__,
                  size);
        goto done;
    }
    if (errno != EPROTO) {
        flux_log (h, LOG_ERR, "%s: get nonexistent payload: %s", __FUNCTION__,
                  strerror (errno));
        goto done;
    }
    errno = 0;
    if (flux_json_respond (h, 0, NULL, zmsg) < 0) {
        flux_log (h, LOG_ERR, "%s: flux_json_respond: %s", __FUNCTION__,
                  strerror (errno));
        goto done;
    }
    rc = 0;
done:
    return rc;
}

static msghandler_t htab[] = {
    { FLUX_MSGTYPE_REQUEST, "req.null",              null_request_cb },
};
const int htablen = sizeof (htab) / sizeof (htab[0]);

int mod_main (flux_t h, zhash_t *args)
{
    ctx_t *ctx = getctx (h);

    if (flux_msghandler_addvec (h, htab, htablen, ctx) < 0) {
        flux_log (h, LOG_ERR, "flux_msghandler_addvec: %s", strerror (errno));
        return -1;
    }
    if (flux_reactor_start (h) < 0) {
        flux_log (h, LOG_ERR, "flux_reactor_start: %s", strerror (errno));
        return -1;
    }
    return 0;
}

MOD_NAME ("req");

/*
 * vi:tabstop=4 shiftwidth=4 expandtab
 */
