#include "WidgetMgr_dep.h"
#include "EffectMgr.h"
#include "TextureMgr.h"
#include "RenderStateMgr.h"

WidgetMgr_deprecated::WidgetMgr_deprecated()
{
    mDevice = 0;
    mCtx = 0;

    mGraphicsVB = 0;
    mGraphicsIB = 0;

    mFontAtlas = 0;
    mTextVB = 0;
    mTextIB = 0;
    mTextQuadNum = 0;
}

WidgetMgr_deprecated::~WidgetMgr_deprecated()
{
    clearGraphicsBuffers();
    clearTextBuffers();
    for (UINT i = 0; i < mWidgets.size(); ++i)
    {
        if (mWidgets[i])
        {
            delete mWidgets[i];
            mWidgets[i] = 0;
        }
    }
}

bool WidgetMgr_deprecated::init( ID3D11Device* device, ID3D11DeviceContext* context, Mouse* pMouse )
{
    mDevice = device;
    mCtx = context;
    mpMouse = pMouse;

    // Hook handlers to mouse events
    mpMouse->EventMove.hook(MouseEvent::SIGNAL_TYPE::slot_type(&WidgetMgr_deprecated::onMouseMove, this, _1, _2));

    // Init font
    mFontEngine.createFontSheet(device, mFontSheet, L"debug");
    mFontAtlas = TextureMgr::getTexture(mFontSheet.mFontSheetBmpFileName);

    return true;
}

void WidgetMgr_deprecated::setMouse( Mouse* pMouse )
{
    mpMouse = pMouse;
}

void WidgetMgr_deprecated::drawUI()
{
    updateGraphicsBuffers();

    mCtx->IASetInputLayout(InputLayoutMgr::OverlayVertex);
    mCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(Vertex::OverlayVertex);
    UINT offset = 0;
    mCtx->IASetVertexBuffers(0, 1, &mGraphicsVB, &stride, &offset);
    mCtx->IASetIndexBuffer(mGraphicsIB, DXGI_FORMAT_R32_UINT, 0);

    // TODO: enrich the UI effect
    EffectMgr::OverlayFX->OverlayTech->GetPassByIndex(0)->Apply(0, mCtx);

    // TODO: blending code not working, why?
    //float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
    //mCtx->OMSetBlendState(RenderStateMgr::TransparentBS, blendFactor, 0xffffffff);
    mCtx->DrawIndexed(mWidgets.size()*6, 0, 0);
    //mCtx->OMSetBlendState(0, blendFactor, 0xffffffff);

    drawText();

}

void WidgetMgr_deprecated::batchBegin()
{
}

void WidgetMgr_deprecated::batchEnd()
{
    clearGraphicsBuffers();
    createGraphicsBuffers();

    clearTextBuffers();
    createTextBuffers();
}

WidgetBase* WidgetMgr_deprecated::createWidget( float posX, float poxY, int width, int height, std::wstring& labelText, std::wstring widgetType /*= L"Widget/Button"*/ )
{
    WidgetBase* pWidget = 0;

    D3D11_VIEWPORT vp;
    UINT vpNum = 1;
    mCtx->RSGetViewports(&vpNum, &vp);

    if (widgetType.compare(L"Widget/Button") == 0)
    {
        pWidget = new WidgetBase(posX, poxY, width, height, Viewport(vp.Width, vp.Height));
        mWidgets.push_back(pWidget);
        return pWidget;
    }
    else if (widgetType.compare(L"Widget/Text") == 0)
    {
        // TODO...
        return pWidget;
    }
    else
    {
        return pWidget;
    }
}

void WidgetMgr_deprecated::onMouseMove( int x, int y )
{
    for (UINT i = 0; i < mWidgets.size(); ++i)
    {
        if (mWidgets[i]->isPointInside(mpMouse->mLastPosX, mpMouse->mLastPosY) == false &&
            mWidgets[i]->isPointInside(x, y) == true)
        {
            mWidgets[i]->onMouseEnter(x, y);
        }
        else if (mWidgets[i]->isPointInside(mpMouse->mLastPosX, mpMouse->mLastPosY) == true&&
            mWidgets[i]->isPointInside(x, y) == false)
        {
            mWidgets[i]->onMouseLeave(x, y);
        }
        else if(mWidgets[i]->isPointInside(x, y) == true)
        {
            mWidgets[i]->onMouseMove(x, y, mpMouse->mLastPosX, mpMouse->mLastPosY);
        }
        else
        {
            continue;
        }
    }

    mpMouse->mLastPosX = x;
    mpMouse->mLastPosY = y;
}

void WidgetMgr_deprecated::onMouseLBtnDown( int x, int y )
{
    for (UINT i = 0; i < mWidgets.size(); ++i)
    {
        // Only the widget that gets the mouse click receives this click
        if (mWidgets[i]->isPointInside(x, y) == true)
        {
            mWidgets[i]->onLBtnDown(x, y);
        }
    }
}

void WidgetMgr_deprecated::onMouseLBtnUp( int x, int y )
{
    for (UINT i = 0; i < mWidgets.size(); ++i)
    {
        // Only the widget that has focus receives this event
        if (mWidgets[i]->hasFocus() == true)
        {
            mWidgets[i]->onLBtnUp(x, y);
        }
    }
}

void WidgetMgr_deprecated::onMouseRBtnDown( int x, int y )
{
    for (UINT i = 0; i < mWidgets.size(); ++i)
    {
        // Only the widget that gets the mouse click receives this event
        if (mWidgets[i]->isPointInside(x, y) == true)
        {
            mWidgets[i]->onRBtnDown(x, y);
        }
    }
}

void WidgetMgr_deprecated::onMouseRBtnUp( int x, int y )
{
    for (UINT i = 0; i < mWidgets.size(); ++i)
    {
        // Only the widget that gets the mouse click receives this event
        if (mWidgets[i]->isPointInside(x, y) == true)
        {
            mWidgets[i]->onRBtnUp(x, y);
        }
    }
}

void WidgetMgr_deprecated::onMouseWheelUp( int x, int y )
{
    for (UINT i = 0; i < mWidgets.size(); ++i)
    {
        if (mWidgets[i]->hasFocus())
        {
            mWidgets[i]->onWheelUp(x, y);
        }
    }
}

void WidgetMgr_deprecated::onMouseWheelDown( int x, int y )
{
    for (UINT i = 0; i < mWidgets.size(); ++i)
    {
        if (mWidgets[i]->hasFocus())
        {
            mWidgets[i]->onWheelDown(x, y);
        }
    }
}

void WidgetMgr_deprecated::onViewportResize( int width, int height )
{
    for (UINT i = 0; i < mWidgets.size(); ++i)
    {
        mWidgets[i]->onResize(Viewport(width, height));
    }
}

void WidgetMgr_deprecated::createGraphicsBuffers()
{
    // Create a dynamic VB 
    //
    D3D11_BUFFER_DESC vbd;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.ByteWidth = sizeof(Vertex::OverlayVertex) * mWidgets.size() * 4;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;
    vbd.Usage = D3D11_USAGE_DYNAMIC; // make dynamic
    HR(mDevice->CreateBuffer(&vbd, 0, &mGraphicsVB));

    //
    // Create static IB
    //
    std::vector<UINT> indices(mWidgets.size() * 6, 0);
    for (UINT i = 0; i < mWidgets.size(); ++i)
    {
        indices[i*6+0] = i*4+0;
        indices[i*6+1] = i*4+1;
        indices[i*6+2] = i*4+2;
        indices[i*6+3] = i*4+0;
        indices[i*6+4] = i*4+2;
        indices[i*6+5] = i*4+3;
    }

    D3D11_BUFFER_DESC ibd;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.ByteWidth = sizeof(UINT) * mWidgets.size() * 6;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    D3D11_SUBRESOURCE_DATA idata;
    idata.pSysMem = &indices[0];
    HR(mDevice->CreateBuffer(&ibd, &idata, &mGraphicsIB));
}

void WidgetMgr_deprecated::clearGraphicsBuffers()
{
    safe_release(&mGraphicsVB);
    safe_release(&mGraphicsIB);
}

void WidgetMgr_deprecated::updateGraphicsBuffers()
{
    D3D11_MAPPED_SUBRESOURCE mappedData;
    mCtx->Map(mGraphicsVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
    Vertex::OverlayVertex* vStart = reinterpret_cast<Vertex::OverlayVertex*>(mappedData.pData);
    for(UINT i = 0; i < mWidgets.size(); ++i)
    {
        Vertex::OverlayVertex* pV = mWidgets[i]->getVerticesInNDC();
        vStart[4*i + 0] = pV[0];
        vStart[4*i + 1] = pV[1];
        vStart[4*i + 2] = pV[2];
        vStart[4*i + 3] = pV[3];
    }
    mCtx->Unmap(mGraphicsVB, 0);
}

void WidgetMgr_deprecated::createTextBuffers()
{
    // Each char is represented by a text quad
    // Have to figure out how many text quads are needed
    UINT textQuadNum= 0;
    for (UINT i = 0; i < mWidgets.size(); ++i)
    {
        textQuadNum += mWidgets[i]->getText().size();
    }

    mTextQuadNum = textQuadNum;

    // Create a dynamic VB 
    //
    D3D11_BUFFER_DESC vbd;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.ByteWidth = sizeof(Vertex::OverlayVertex) * textQuadNum * 4;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;
    vbd.Usage = D3D11_USAGE_DYNAMIC; // make dynamic
    HR(mDevice->CreateBuffer(&vbd, 0, &mTextVB));

    //
    // Create static IB
    //
    std::vector<UINT> indices(textQuadNum * 6, 0);
    for (UINT i = 0; i < textQuadNum; ++i)
    {
        indices[i*6+0] = i*4+0;
        indices[i*6+1] = i*4+1;
        indices[i*6+2] = i*4+2;
        indices[i*6+3] = i*4+0;
        indices[i*6+4] = i*4+2;
        indices[i*6+5] = i*4+3;
    }

    D3D11_BUFFER_DESC ibd;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.ByteWidth = sizeof(UINT) * textQuadNum * 6;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    D3D11_SUBRESOURCE_DATA idata;
    idata.pSysMem = &indices[0];
    HR(mDevice->CreateBuffer(&ibd, &idata, &mTextIB));
}

void WidgetMgr_deprecated::clearTextBuffers()
{
    safe_release(&mTextVB);
    safe_release(&mTextIB);
}

void WidgetMgr_deprecated::updateTextBuffers()
{
    D3D11_MAPPED_SUBRESOURCE mappedData;
    mCtx->Map(mTextVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
    Vertex::OverlayVertex* vStart = reinterpret_cast<Vertex::OverlayVertex*>(mappedData.pData);

    std::wstring textToDraw;
    UINT totalCharNum = 0;
    RectFloat textArea;
    
    for (UINT i = 0;i < mWidgets.size(); ++i)
    {
        textToDraw = mWidgets[i]->getText();
        mWidgets[i]->getTextArea(textArea);

        XMFLOAT2 textAreaLeftTop(textArea.left, textArea.top);
        float textAreaWidth = (textArea.right - textArea.left);
        float textAreaHeight = (textArea.top - textArea.bottom);

        XMFLOAT2 insertPos(0.f, 0.f);

        float resizeRatio = mWidgets[i]->getFontSize()/mFontSheet.mCharHeight;
        float vpWidth = static_cast<float>(mWidgets[i]->getViewPort().width);
        float vpHeight = static_cast<float>(mWidgets[i]->getViewPort().height);

        // For each character...
        for (UINT j = 0; j < textToDraw.size(); ++j)
        {
            WCHAR c = textToDraw[j];

            // start a new line if \n
            if (c == 10)
            {
                insertPos.x = 0;
                insertPos.y -= 2.f * (mFontSheet.mCharHeight) * resizeRatio / vpHeight;
            }

            // Get src rect
            CD3D11_RECT srcRect = *(mFontSheet.getSrcRect(c));

            // Compute dst rect
            // Text resize in pixel unit, then transformed to NDC space
            float dstRectHeight = 2.f * (srcRect.bottom - srcRect.top) * resizeRatio / vpHeight;
            float dstRectWidth = 2.f * (srcRect.right - srcRect.left) * resizeRatio/ vpWidth;

            // Construct the quad
            Vertex::OverlayVertex v[4];

            // NDC space
            v[0].Pos = XMFLOAT3(insertPos.x,                insertPos.y,                    gDefaultWidgetDepth2);
            v[1].Pos = XMFLOAT3(insertPos.x + dstRectWidth, insertPos.y,                    gDefaultWidgetDepth2);
            v[2].Pos = XMFLOAT3(insertPos.x + dstRectWidth, insertPos.y - dstRectHeight,    gDefaultWidgetDepth2);
            v[3].Pos = XMFLOAT3(insertPos.x,                insertPos.y - dstRectHeight,    gDefaultWidgetDepth2);

            // Texture space
            v[0].Tex = XMFLOAT2(static_cast<float>(srcRect.left)/mFontSheet.mTextureWidth, static_cast<float>(srcRect.top)/mFontSheet.mTextureHeight);
            v[1].Tex = XMFLOAT2(static_cast<float>(srcRect.right)/mFontSheet.mTextureWidth, static_cast<float>(srcRect.top)/mFontSheet.mTextureHeight);
            v[2].Tex = XMFLOAT2(static_cast<float>(srcRect.right)/mFontSheet.mTextureWidth, static_cast<float>(srcRect.bottom)/mFontSheet.mTextureHeight);
            v[3].Tex = XMFLOAT2(static_cast<float>(srcRect.left)/mFontSheet.mTextureWidth, static_cast<float>(srcRect.bottom)/mFontSheet.mTextureHeight);

            // Translate the quad into the textArea 
            for (UINT vi = 0; vi < 4; ++vi)
            {
                v[vi].Pos.x += textAreaLeftTop.x;  
                v[vi].Pos.y += textAreaLeftTop.y;  
            }

            // Save the quad to mappedData
            vStart[4*totalCharNum + 0] = v[0];
            vStart[4*totalCharNum + 1] = v[1];
            vStart[4*totalCharNum + 2] = v[2];
            vStart[4*totalCharNum + 3] = v[3];

            // Keep track of touched char number
            totalCharNum++;

            // Step insertPos
            insertPos.x += dstRectWidth;
            // Start a new line
            if (insertPos.x > textAreaWidth)
            {
                insertPos.x = 0;
                insertPos.y -= dstRectHeight;
            }
        }
    }
    mCtx->Unmap(mTextVB, 0);
}

void WidgetMgr_deprecated::drawText()
{
    updateTextBuffers();

    mCtx->IASetInputLayout(InputLayoutMgr::OverlayVertex);
    mCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(Vertex::OverlayVertex);
    UINT offset = 0;
    mCtx->IASetVertexBuffers(0, 1, &mTextVB, &stride, &offset);
    mCtx->IASetIndexBuffer(mTextIB, DXGI_FORMAT_R32_UINT, 0);

    // TODO: enrich the UI effect
    //EffectMgr::OverlayFX->OverlayTech->GetPassByIndex(0)->Apply(0, mCtx);
    EffectMgr::OverlayFX->setDiffuseMap(mFontAtlas);
    EffectMgr::OverlayFX->OverlayTexTech->GetPassByIndex(0)->Apply(0, mCtx);

    mCtx->DrawIndexed(mTextQuadNum*6, 0, 0);

}




