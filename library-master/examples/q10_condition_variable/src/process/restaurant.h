#ifndef CONDITION_VARIABLE_RESTAURANT_H
#define CONDITION_VARIABLE_RESTAURANT_H

#define CONDITION_VARIABLE_RESTAURANT_CV_NAME  "condition_variable_restaurant_cv"
#define CONDITION_VARIABLE_RESTAURANT_MUTEX_NAME "condition_variable_restaurant_mutex"
#define CONDITION_VARIABLE_RESTAURANT_DATA_NAME "condition_variable_data_name"

struct DishData {
  int dirty;
  int clean;
  int washing;
  bool quit;
};

#endif //CONDITION_VARIABLE_RESTAURANT_H
