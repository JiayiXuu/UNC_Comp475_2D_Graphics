typedef float (*TileFunction)(float t);

static inline float Clamp(float t)
{
    return std::max(0.0f, std::min(0.9999999f, t));
}

static inline float Repeat(float t)
{
    return t - GFloorToInt(t);

}
static inline float Mirror(float t)
{
    t *= 0.5;
    t = t - GFloorToInt(t);
    if (t>0.5){t = 1-t;}
    return t *= 2;

}