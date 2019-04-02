#ifndef LPI_GPIO_H
#define LPI_GPIO_H

int gpio_export(unsigned int gpio);

int gpio_unexport(unsigned int gpio);

int gpio_set_dir(unsigned int gpio,int out_flag);

int gpio_get_dir(unsigned int gpio,int *out_flag);

int gpio_set_value(unsigned int gpio,unsigned int value);

int gpio_get_value(unsigned int gpio,unsigned int *value);

int gpio_set_edge(unsigned int gpio,unsigned int edge_flag);

#endif // LPI_GPIO_H
