class BoundingBox{
    public:
    float x;
    float y;
    float height;
    float width;
    float motion;
};

class CoinBox : public BoundingBox{
    public:
    CoinBox(float gap_size, float coin_size);
    int ** map;
    int nvert;
    int nhorz;
};

class LaserBox : public BoundingBox{
    public:
    LaserBox();
    float angle;
    int rotate;
};