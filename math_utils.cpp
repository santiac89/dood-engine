#include "math_utils.h"

int distance(int x1, int y1, int x2, int y2) {
  return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

int atan2b(int y, int x) {
  return M_PI + atan2(-y,-x);
}

float angle(vector<float> v1, vector<float> v2) {
  float dot = v1[0]*v2[0] + v1[1]*v2[1];
  float det = v1[0]*v2[1] - v1[1]*v2[0];
  return atan2(det, dot);
}

vector<float> angleToVector(float angle) {
    vector<float> result(2);
    result[0] = cos(angle);
    result[1] = sin(angle);
    return result;
}

vector<float> unitVector(vector<float> v) {
  vector<float> unitVector(2);
  unitVector[0] = v[0] / magnitude(v);
  unitVector[1] = v[1] / magnitude(v);
  return unitVector;
}

float magnitude(vector<float> v) {
  return sqrt(v[0] * v[0] + v[1] * v[1]);
}
 
// double atan2_r(double angle) {
//     return atan2(2*cos(angle), -1.5*sin(angle));
// }