#include "Deadzone.h"

bool Deadzone::isEnabled() { return deadzoneFactor > 0; }

float Deadzone::getValue() { return DEADZONE_STEP_VALUE * deadzoneFactor; }

void Deadzone::increase() {
  if (deadzoneFactor < DEADZONE_FACTOR_MAX) {
    ++deadzoneFactor;
  }
}

void Deadzone::decrease() {
  if (deadzoneFactor > DEADZONE_FACTOR_MIN) {
    --deadzoneFactor;
  }
}

void Deadzone::apply(float* const x, float* const y) {
  if (!isEnabled()) {
    return;
  }

  float deadzone = getValue();

  // calculate maginitude: c² = a² + b²
  // this approximation (0 ≤ a ≤ b) is faster and gives us max error of 1.04%:
  float xAbs = abs(*x);
  float yAbs = abs(*y);
  float a = min(xAbs, yAbs);
  float b = max(xAbs, yAbs);
  float magnitude = b + (0.428 * a * a / b);
  if (magnitude < deadzone) {
    *x = 0;
    *y = 0;
  } else {
    float scalingFactor = ((magnitude - deadzone) / (1.0f - deadzone));
    xAbs = (xAbs / magnitude) * scalingFactor;
    yAbs = (yAbs / magnitude) * scalingFactor;
    *x = (*x < 0) ? max(-xAbs, -1) : min(xAbs, 1);
    *y = (*y < 0) ? max(-yAbs, -1) : min(yAbs, 1);
  }
}
