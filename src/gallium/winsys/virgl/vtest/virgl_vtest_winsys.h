#ifndef VIRGL_DRM_WINSYS_H
#define VIRGL_DRM_WINSYS_H

#include <stdint.h>
#include "pipe/p_compiler.h"
#include "pipe/p_defines.h"
#include "pipe/p_state.h"
#include "state_tracker/sw_winsys.h"
#include "../drm/virgl_hw.h"
#include "virgl/virgl_winsys.h"
#include "util/u_double_list.h"
#include "os/os_thread.h"

#include "vtest_protocol.h"
struct virgl_vtest_winsys {
   struct virgl_winsys base;

   struct sw_winsys *sws;

   /* fd to remote renderer */
   int sock_fd;

   struct list_head delayed;
   int num_delayed;
   unsigned usecs;
   pipe_mutex mutex;
};

struct virgl_hw_res {
   struct pipe_reference reference;
   uint32_t res_handle;
   int num_cs_references;

   void *ptr;
   int size;

   uint32_t format;
   uint32_t stride;
   uint32_t width;
   uint32_t height;

   struct sw_displaytarget *dt;
   void *mapped;

   struct list_head head;
   uint32_t bind;
   boolean cacheable;
   int64_t start, end;

};

struct virgl_vtest_cmd_buf {
   struct virgl_cmd_buf base;
   uint32_t buf[VIRGL_MAX_CMDBUF_DWORDS];
   unsigned nres;
   unsigned cres;
   struct virgl_winsys *ws;
   struct virgl_hw_res **res_bo;

   char                        is_handle_added[512];
   unsigned                    reloc_indices_hashlist[512];
};

static INLINE struct virgl_vtest_winsys *
virgl_vtest_winsys(struct virgl_winsys *iws)
{
   return (struct virgl_vtest_winsys *)iws;
}

int virgl_vtest_connect(struct virgl_vtest_winsys *vws);
int virgl_vtest_send_get_caps(struct virgl_vtest_winsys *vws,
                              struct virgl_drm_caps *caps);

int virgl_vtest_send_resource_create(struct virgl_vtest_winsys *vws,
                                     uint32_t handle,
                                     enum pipe_texture_target target,
                                     uint32_t format,
                                     uint32_t bind,
                                     uint32_t width,
                                     uint32_t height,
                                     uint32_t depth,
                                     uint32_t array_size,
                                     uint32_t last_level,
                                     uint32_t nr_samples);

int virgl_vtest_send_resource_unref(struct virgl_vtest_winsys *vws,
                                    uint32_t handle);
int virgl_vtest_submit_cmd(struct virgl_vtest_winsys *vtws,
                           struct virgl_vtest_cmd_buf *cbuf);

int virgl_vtest_send_transfer_cmd(struct virgl_vtest_winsys *vws,
                                  uint32_t vcmd,
                                  uint32_t handle,
                                  uint32_t level, uint32_t stride,
                                  uint32_t layer_stride,
                                  const struct pipe_box *box,
                                  uint32_t data_size);

int virgl_vtest_send_transfer_put_data(struct virgl_vtest_winsys *vws,
                                       void *data,
                                       uint32_t data_size);
int virgl_vtest_recv_transfer_get_data(struct virgl_vtest_winsys *vws,
                                       void *data,
                                       uint32_t data_size,
                                       uint32_t stride,
                                       const struct pipe_box *box, uint32_t format);

int virgl_vtest_busy_wait(struct virgl_vtest_winsys *vws, int handle,
                          int flags);
#endif
