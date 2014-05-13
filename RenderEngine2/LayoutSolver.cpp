#include "LayoutSolver.h"
#include "Widget.h"
#include "GUIRenderer.h"

void LayoutSolver::solve( Widget* widget)
{
    solveImpl(widget);
    mIsLayoutUpdated = true;
}

void LayoutSolver::solveImpl( Widget* widget )
{
    Box2D clientBox = widget->getClientArea();

    if (widget->mTextBlock.hasText())
    {
        widget->mTextBlock.solveLayout(clientBox, widget->mLayerDepth + 1);
    }

    Point2D insertPos = clientBox.point[0];
    std::vector<int> lineHeights;

    UINT defaultMargin = 2;

    UINT childNum = widget->mChildren.size();
    for (UINT i = 0; i < childNum; ++i)
    {
        Widget* child = widget->mChildren[i];
        switch(child->mLayoutType)
        {
        case WIDGET_LAYOUT_VERTICAL:
            child->mPos = Point2D(insertPos.x + defaultMargin, insertPos.y);
            child->mAbsolutePos = child->mPos + widget->mAbsolutePos;
            child->mLayerDepth = widget->mLayerDepth + 1;

            insertPos.y += child->mSize.height + defaultMargin;
            break;

        case WIDGET_LAYOUT_STREAM:
            // Stream into a new line
            if (insertPos.x + child->mSize.width > clientBox.getWidth())
            {
                insertPos.x = 0;
                insertPos.y += getMax(lineHeights, 5) + defaultMargin;
                lineHeights.clear();
            }
            child->mPos = insertPos;
            child->mAbsolutePos = child->mPos + widget->mAbsolutePos;
            child->mLayerDepth = widget->mLayerDepth + 1;

            insertPos.x += child->mSize.width + defaultMargin;
            lineHeights.push_back(child->mSize.height);

            break;

        case WIDGET_LAYOUT_CENTER:
            break;

        case WIDGET_LAYOUT_DOCK_LEFT_AUTO_HEIGHT:
            child->mPos.x = clientBox.getLeft();
            child->mPos.y = clientBox.getTop();
            child->mAbsolutePos = child->mPos + widget->mAbsolutePos;

            child->mSize.height = clientBox.getHeight();

            child->mLayerDepth = widget->mLayerDepth + 1;
            break;

        default:
            break;
        }
    }

    for (UINT i = 0; i < childNum; ++i)
    {
        Widget* child = widget->mChildren[i];
        solveImpl(child);
    }
}

int LayoutSolver::getMax( std::vector<int>& v, int defaultMax )
{
    if (v.size() == 0)
        return defaultMax;

    int max = v[0];
    for (UINT i = 0; i < v.size(); ++i)
        if (max < v[i]) 
            max = v[i];

    return max;
}
