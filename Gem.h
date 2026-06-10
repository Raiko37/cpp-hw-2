#pragma once

#include "Color.h"

class Board;

class Gem {
public:
    virtual ~Gem() = default;

    virtual Color getColor() const = 0;

    virtual void activate(Board& board, int x, int y);
};

class StandardGem : public Gem {
public:
    explicit StandardGem(Color color);

    Color getColor() const override;

private:
    Color color;
};

class BonusGem : public Gem {
public:
    explicit BonusGem(Color sourceColor);

    Color getColor() const override;

protected:
    Color sourceColor;
};

class RecolorBonusGem : public BonusGem {
public:
    explicit RecolorBonusGem(Color sourceColor);

    void activate(Board& board, int x, int y) override;
};

class BombBonusGem : public BonusGem {
public:
    explicit BombBonusGem(Color sourceColor);

    void activate(Board& board, int x, int y) override;
};