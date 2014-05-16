#ifndef LAYOUT_SOLVER_H
#define LAYOUT_SOLVER_H
#include <vector>

class Widget;
class GUIRenderer;
/*
enum LayoutType
{
    WIDGET_LAYOUT_CENTER = 0,
    WIDGET_LAYOUT_STREAM,
    WIDGET_LAYOUT_VERTICAL,
    WIDGET_LAYOUT_HORIZENAL,
    WIDGET_LAYOUT_DOCK_LEFT_AUTO_HEIGHT,
    WIDGET_LAYOUT_NOT_SPECIFIED
};
*/

class LayoutSolver
{
public:
    // Solver holds a ref to renderer to inform it when layout is re-solved
    void solve(Widget* widget);

    bool updated()
    {
        return mIsLayoutUpdated;
    }
    void resetUpdated()
    {
        mIsLayoutUpdated = false;
    }

private:
    void solveImpl(Widget* widget);
    int getMax(std::vector<int>& v, int defaultMax);
    bool mIsLayoutUpdated;
};

#endif