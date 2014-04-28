#ifndef OVERLAY_GRAPHICS_H
#define OVERLAY_GRAPHICS_H

#include "Util.h"
#include "InputLayoutMgr.h"
#include "Timer.h"


class Animation2D
{
public:
    Animation2D()
    {
        Keyframe2D f0;
        f0.time = 0.f;
        f0.pos = XMFLOAT2(0.f, 0.f);

        Keyframe2D f1;
        f1.time = 0.3f;
        f1.pos = XMFLOAT2(0.05f, 0.1f);

        Keyframe2D f2;
        f2.time = 0.6f;
        f2.pos = XMFLOAT2(0.0f, 0.0f);

        mKeyframes.push_back(f0);
        mKeyframes.push_back(f1);
        mKeyframes.push_back(f2);
    }

    void start()
    {
        mTimer.start();
    }

    XMFLOAT2 loop()
    {
        mTimer.tick();
        XMFLOAT2 pos = XMFLOAT2(0.f, 0.f);

        if (mTimer.totalTime() > mKeyframes[mKeyframes.size() -1].time)
        {
            pos.x = mKeyframes[mKeyframes.size() -1 ].pos.x;
            pos.y = mKeyframes[mKeyframes.size() -1 ].pos.y;
            mTimer.reset();
        }
        else
        {
            // Interpolating key-framed animation
            for (UINT i = 1; i < mKeyframes.size(); ++i)
            {
                if (mTimer.totalTime() <= mKeyframes[i].time &&
                    mTimer.totalTime() >= mKeyframes[i-1].time)
                {
                    float timeFactor = (mTimer.totalTime() - mKeyframes[i-1].time)/(mKeyframes[i].time - mKeyframes[i-1].time);
                    pos.x = (mKeyframes[i].pos.x - mKeyframes[i-1].pos.x)*timeFactor + mKeyframes[i-1].pos.x;
                    pos.y = (mKeyframes[i].pos.y - mKeyframes[i-1].pos.y)*timeFactor + mKeyframes[i-1].pos.y;
                    break;
                }
            }
        }
        return pos;
    }

protected:
    struct Keyframe2D
    {
        float time;
        XMFLOAT2 pos;
    };
    std::vector<Keyframe2D> mKeyframes;
    Timer mTimer;
};

namespace OverlayUI
{
    // Contains geometry info
    class Quad
    {
    public:
        Quad();
        ~Quad();

        Quad(XMFLOAT2 ndcPos, XMFLOAT2 ndcSize, XMFLOAT4 color = XMFLOAT4(0.3f, 0.3f, 0.7f, 0.8f), float depth = 0.1f);

        void updateNdcPos(XMFLOAT2 ndcPos);

        void updateNdcSize(XMFLOAT2 ndcSize);

        Vertex::OverlayVertex* getKeyFrameVertices(){return &mKeyFrameVertices[0];}

    protected:
        void transformToKeyFrame();

    protected:
        Vertex::OverlayVertex mVertices[4];
        Vertex::OverlayVertex mKeyFrameVertices[4];
        XMFLOAT4 mPivot;
        XMFLOAT4 mTranslation;
        XMFLOAT4 mScaling;
    };

    // Render the quads
    class OverlayGraphics
    {
    public:
        OverlayGraphics();
        ~OverlayGraphics();
        bool init(ID3D11Device* dv, ID3D11DeviceContext* ctx);

        Quad* createQuad(XMFLOAT2 ndcPos, XMFLOAT2 ndcSize);

        void beginBatch();
        void endBatch();
        void drawOverlay();

    private:
        void createGraphicBuffers();
        void clearQuads();
        void clearGraphicBuffers();

    private:
        ID3D11Device* mDevice;
        ID3D11DeviceContext* mCtx;

        ID3D11Buffer* mVB;
        ID3D11Buffer* mIB;

        std::vector<Quad*> mQuads;
    };
}


#endif