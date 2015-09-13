void mat_frustum (float *matrix, float angle_of_view, float aspect_ratio, float z_near, float z_far);
void mat_translate (float *matrix, float dx, float dy, float dz);
void mat_rotate (float *matrix, float x, float y, float z, float angle);
void mat_multiply (float *matrix, float *a, float *b);
