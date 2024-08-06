#ifndef TYPES_H
#define TYPES_H

#include <gtk/gtk.h>

typedef struct _user_data_t {
    GtkTreeView *treeview;
    GtkListStore *store;
    GtkBuilder *builder;
} user_data_t;

typedef int16_t word_t;
#define WORD_MAX INT16_MAX

#endif // TYPES_H

