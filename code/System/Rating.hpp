#include <kamek.hpp>

extern float CalcRating(float x);

class Rating
{
    public:
    Rating() { mPoints = 0; }
    virtual ~Rating();

    u16 mPoints;
};