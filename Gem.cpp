#include "Gem.h"
#include "Board.h"

void Gem::activate(Board&, int, int) {
}

StandardGem::StandardGem(Color color)
    : color(color) {
}

Color StandardGem::getColor() const {
    return color;
}

BonusGem::BonusGem(Color sourceColor)
    : sourceColor(sourceColor) {
}

Color BonusGem::getColor() const {
    return sourceColor;
}

RecolorBonusGem::RecolorBonusGem(Color sourceColor)
    : BonusGem(sourceColor) {
}

void RecolorBonusGem::activate(Board& board, int x, int y) {
    board.applyRecolorBonus(x, y, sourceColor);
}

BombBonusGem::BombBonusGem(Color sourceColor)
    : BonusGem(sourceColor) {
}

void BombBonusGem::activate(Board& board, int x, int y) {
    board.applyBombBonus(x, y);
}