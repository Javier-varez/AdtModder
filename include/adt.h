/* SPDX-License-Identifier: (GPL-2.0-or-later OR BSD-2-Clause) */

#ifndef XDT_H
#define XDT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "types.h"

#define ADT_ERR_NOTFOUND 1
#define ADT_ERR_BADOFFSET 4
#define ADT_ERR_BADPATH 5
#define ADT_ERR_BADNCELLS 14
#define ADT_ERR_BADVALUE 15
#define ADT_ERR_BADLENGTH 20

#define ADT_ALIGN 4

extern void *adt;

struct adt_property {
  char name[32];
  u32 size;
  u8 value[];
};

struct adt_node_hdr {
  u32 property_count;
  u32 child_count;
};

#define ADT_NODE(adt, offset)                                                  \
  ((struct adt_node_hdr *)(((u8 *)(adt)) + (offset)))
#define ADT_PROP(adt, offset)                                                  \
  ((struct adt_property *)(((u8 *)(adt)) + (offset)))
#define ADT_SIZE(node) ((node)->size & 0x7fffffff)

/* This API is designed to match libfdt's read-only API */

/* Basic sanity check */
int adt_check_header(void *adt);

static inline int adt_get_property_count(void *adt, int offset) {
  return ADT_NODE(adt, offset)->property_count;
}

static inline int adt_first_property_offset(void *adt, int offset) {
  UNUSED(adt);
  return offset + sizeof(struct adt_node_hdr);
}

static inline int adt_next_property_offset(void *adt, int offset) {
  struct adt_property *prop = ADT_PROP(adt, offset);
  return offset + sizeof(struct adt_property) +
         ((prop->size + ADT_ALIGN - 1) & ~(ADT_ALIGN - 1));
}

static inline struct adt_property *adt_get_property_by_offset(void *adt,
                                                              int offset) {
  return ADT_PROP(adt, offset);
}

static inline int adt_get_child_count(void *adt, int offset) {
  return ADT_NODE(adt, offset)->child_count;
}

int adt_first_child_offset(void *adt, int offset);
int adt_next_sibling_offset(void *adt, int offset);

int adt_subnode_offset_namelen(void *adt, int parentoffset, const char *name,
                               size_t namelen);
int adt_subnode_offset(void *adt, int parentoffset, const char *name);
int adt_path_offset(void *adt, const char *path);
int adt_path_offset_trace(void *adt, const char *path, int *offsets);

const char *adt_get_name(void *adt, int nodeoffset);
struct adt_property *adt_get_property_namelen(void *adt, int nodeoffset,
                                              const char *name, size_t namelen);
struct adt_property *adt_get_property(void *adt, int nodeoffset,
                                      const char *name);
int adt_get_property_offset(void *adt, int nodeoffset, const char *name);
void *adt_getprop_by_offset(void *adt, int offset, const char **namep,
                            u32 *lenp);
void *adt_getprop_namelen(void *adt, int nodeoffset, const char *name,
                          size_t namelen, u32 *lenp);
void *adt_getprop(void *adt, int nodeoffset, const char *name, u32 *lenp);
int adt_getprop_copy(void *adt, int nodeoffset, const char *name, void *out,
                     size_t len);

#define ADT_GETPROP(adt, nodeoffset, name, val)                                \
  adt_getprop_copy(adt, nodeoffset, name, (val), sizeof(*(val)))

#define ADT_GETPROP_ARRAY(adt, nodeoffset, name, arr)                          \
  adt_getprop_copy(adt, nodeoffset, name, (arr), sizeof(arr))

int adt_get_reg(void *adt, int *path, const char *prop, int idx, u64 *addr,
                u64 *size);
bool adt_is_compatible(void *adt, int nodeoffset, const char *compat);

#define ADT_FOREACH_CHILD(adt, node)                                           \
  for (int _child_count = adt_get_child_count(adt, node); _child_count;        \
       _child_count = 0)                                                       \
    for (node = adt_first_child_offset(adt, node); _child_count--;             \
         node = adt_next_sibling_offset(adt, node))

#define ADT_FOREACH_PROPERTY(adt, node, prop)                                  \
  for (int _prop_count = adt_get_property_count(adt, node),                    \
           _poff = adt_first_property_offset(adt, node);                       \
       _prop_count; _prop_count = 0)                                           \
    for (struct adt_property *prop = ADT_PROP(adt, _poff); _prop_count--;      \
         prop = ADT_PROP(adt, _poff = adt_next_property_offset(adt, _poff)))

#ifdef __cplusplus
}
#endif

#endif
