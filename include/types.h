#ifndef TYPES_H
#define TYPES_H

#include <gtk/gtk.h>
#include <stdint.h>

typedef struct _user_data_t {
    GtkTreeView *treeview;
    GtkListStore *store;
    GtkBuilder *builder;
} user_data_t;

typedef int16_t word_t;
#define WORD_MAX INT16_MAX
#define MOP0 0
#define MOP1 1
#define MOP2 2
#define MOP3 3
#define MOP_NOT_DEFINED (-1)

#endif // TYPES_H

