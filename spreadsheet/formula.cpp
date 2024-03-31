#include "formula.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

std::ostream &operator<<(std::ostream &output, FormulaError fe) {
    return output << fe.ToString();
}

// Реализуйте следующие методы:
Formula::Formula(std::string expression) try : ast_(ParseFormulaAST(expression)) {
} catch (const std::exception &er) {
    throw FormulaException(er.what());
}

Formula::Value Formula::Evaluate(const SheetInterface &sheet) const {
    Value result;
    try {
        result.emplace<double>(ast_.Execute(sheet));
        if (std::get<double>(result) == std::numeric_limits<double>::infinity() ||
            std::get<double>(result) == -std::numeric_limits<double>::infinity()) {
            result.emplace<FormulaError>(FormulaError::Category::Arithmetic);
        }
    } catch (const FormulaError &fe) {
        result.emplace<FormulaError>(fe);
    }
    return result;
}
std::string Formula::GetExpression() const {
    std::ostringstream out;
    ast_.PrintFormula(out);
    return out.str();
}

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try {
        return std::make_unique<Formula>(std::move(expression));
    } catch (const std::exception &er) {
        throw FormulaException("Syntax error: " + std::string{er.what()});
    }
}

std::vector<Position> Formula::GetReferencedCells() const {
    std::vector<Position> result;
    for (const auto &cell : ast_.GetCells()) {
        result.push_back(cell);
    }
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    return result;
}