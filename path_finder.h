#ifndef PATH_FINDER_H
#define PATH_FINDER_H


void print_path_map(path_node_t *path_arr);
void update_tunn_map(path_node_t *path_arr, cell_t *cell_arr);
void update_non_tunn_map(path_node_t *path_arr, cell_t *cell_arr);
 

#endif
