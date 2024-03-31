#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "cell.h"
#include "common.h"

class Sheet : public SheetInterface {
   public:
    Sheet() = default;
    ~Sheet() = default;

    // void SetCell(const Position &pos);

    void SetCell(Position pos, std::string text) override;

    const CellInterface *GetCell(Position pos) const override;
    CellInterface *GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream &output) const override;
    void PrintTexts(std::ostream &output) const override;

    // Можете дополнить ваш класс нужными полями и методами

   private:
    Size size_;
    std::vector<std::vector<std::unique_ptr<Cell>>> sheet_;
};