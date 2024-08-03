#ifndef TYPES_H
#define TYPES_H

#include <gtk/gtk.h>

typedef struct {
    GtkTreeView *treeview;
    GtkListStore *store;
    GtkBuilder *builder;
} user_data;

#endif // TYPES_H

