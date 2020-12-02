typedef GPixel (*BlendFunc)(GPixel src, GPixel dst);

GPixel clear_mode(GPixel src, GPixel dst)
{
    return GPixel_PackARGB(0, 0, 0, 0);
}

GPixel Src_mode(GPixel src, GPixel dst)
{
    return src;
}

GPixel Dst_mode(GPixel src, GPixel dst)
{
    return dst;
}

GPixel SrcOver_mode(GPixel src, GPixel dst)
{
    int S_alpha = 255 - GPixel_GetA(src);
    return GPixel_PackARGB((GPixel_GetA(src) + Div255Shift(S_alpha * GPixel_GetA(dst))),
                           (GPixel_GetR(src) + Div255Shift(S_alpha * GPixel_GetR(dst))),
                           (GPixel_GetG(src) + Div255Shift(S_alpha * GPixel_GetG(dst))),
                           (GPixel_GetB(src) + Div255Shift(S_alpha * GPixel_GetB(dst))));
}

GPixel DstOver_mode(GPixel src, GPixel dst)
{
    int D_alpha = 255 - GPixel_GetA(dst);
    if(D_alpha>=255) {return src;}
    if(D_alpha<=0) {return dst;}
    return GPixel_PackARGB((GPixel_GetA(dst) + Div255Shift(D_alpha * GPixel_GetA(src))),
                           (GPixel_GetR(dst) + Div255Shift(D_alpha * GPixel_GetR(src))),
                           (GPixel_GetG(dst) + Div255Shift(D_alpha * GPixel_GetG(src))),
                           (GPixel_GetB(dst) + Div255Shift(D_alpha * GPixel_GetB(src))));
}

GPixel SrcIn_mode(GPixel src, GPixel dst)
{
    int Da = GPixel_GetA(dst);
    if (Da<=0) {return GPixel_PackARGB(0, 0, 0, 0);}
    if (Da>=255){return src;}
    return GPixel_PackARGB(Div255Shift(Da * GPixel_GetA(src)),
                           Div255Shift(Da * GPixel_GetR(src)),
                           Div255Shift(Da * GPixel_GetG(src)),
                           Div255Shift(Da * GPixel_GetB(src)));
}

GPixel DstIn_mode(GPixel src, GPixel dst)
{
    int Sa = GPixel_GetA(src);
    return GPixel_PackARGB(Div255Shift(Sa * GPixel_GetA(dst)),
                           Div255Shift(Sa * GPixel_GetR(dst)),
                           Div255Shift(Sa * GPixel_GetG(dst)),
                           Div255Shift(Sa * GPixel_GetB(dst)));
}

GPixel SrcOut_mode(GPixel src, GPixel dst)
{
    int D_alpha = 255 - GPixel_GetA(dst);
    if (D_alpha>=255) {return src;}
    if(D_alpha<=0) {return GPixel_PackARGB(0, 0, 0, 0);}
    return GPixel_PackARGB(Div255Shift(D_alpha * GPixel_GetA(src)),
                           Div255Shift(D_alpha * GPixel_GetR(src)),
                           Div255Shift(D_alpha * GPixel_GetG(src)),
                           Div255Shift(D_alpha * GPixel_GetB(src)));
}

GPixel DstOut_mode(GPixel src, GPixel dst)
{
    int S_alpha = 255 - GPixel_GetA(src);
    return GPixel_PackARGB(Div255Shift(S_alpha * GPixel_GetA(dst)),
                           Div255Shift(S_alpha * GPixel_GetR(dst)),
                           Div255Shift(S_alpha * GPixel_GetG(dst)),
                           Div255Shift(S_alpha * GPixel_GetB(dst)));
}

GPixel SrcATop_mode(GPixel src, GPixel dst)
{
    int Da = GPixel_GetA(dst);
    if(Da<=0){return GPixel_PackARGB(0, 0, 0, 0);}
    int S_alpha = 255 - GPixel_GetA(src);
    return GPixel_PackARGB(Div255Shift(Da * GPixel_GetA(src) + S_alpha * GPixel_GetA(dst)),
                           Div255Shift(Da * GPixel_GetR(src) + S_alpha * GPixel_GetR(dst)),
                           Div255Shift(Da * GPixel_GetG(src) + S_alpha * GPixel_GetG(dst)),
                           Div255Shift(Da * GPixel_GetB(src) + S_alpha * GPixel_GetB(dst)));
}

GPixel DstATop_mode(GPixel src, GPixel dst)
{   
    int D_alpha = 255 - GPixel_GetA(dst);
    if(D_alpha>=255){return src;}
    if(D_alpha<=0){return DstIn_mode(src,dst);}
    int Sa = GPixel_GetA(src);
    return GPixel_PackARGB(Div255Shift(D_alpha * GPixel_GetA(src) + Sa * GPixel_GetA(dst)),
                           Div255Shift(D_alpha * GPixel_GetR(src) + Sa * GPixel_GetR(dst)),
                           Div255Shift(D_alpha * GPixel_GetG(src) + Sa * GPixel_GetG(dst)),
                           Div255Shift(D_alpha * GPixel_GetB(src) + Sa * GPixel_GetB(dst)));
}

GPixel Xor_mode(GPixel src, GPixel dst)
{
    int D_alpha = 255 - GPixel_GetA(dst);
    if(D_alpha>=255){return src;}
    if(D_alpha<=0){return DstOut_mode(src,dst);}
    int S_alpha = 255 - GPixel_GetA(src);
    return GPixel_PackARGB(Div255Shift(D_alpha * GPixel_GetA(src) + S_alpha * GPixel_GetA(dst)),
                           Div255Shift(D_alpha * GPixel_GetR(src) + S_alpha * GPixel_GetR(dst)),
                           Div255Shift(D_alpha * GPixel_GetG(src) + S_alpha * GPixel_GetG(dst)),
                           Div255Shift(D_alpha * GPixel_GetB(src) + S_alpha * GPixel_GetB(dst)));
}


BlendFunc switch_mode(GPaint src,GShader *shader,bool isOpaque)
{
    GBlendMode mode = src.getBlendMode();
    float Src_alpha = src.getAlpha();
    if (mode == GBlendMode::kClear){
        return clear_mode;
    }
    if (mode == GBlendMode::kSrc)
    {
        if (Src_alpha<= 0 && shader==nullptr) {return clear_mode;}
        return Src_mode;
    }
    if (mode == GBlendMode::kDst){return Dst_mode;}
    if (mode == GBlendMode::kSrcOver)
    {
        if (Src_alpha <= 0 && shader==nullptr) {return Dst_mode;}
        if ((Src_alpha >= 1 && shader==nullptr) ||  (isOpaque && shader!=nullptr) ) {return Src_mode;}
        return SrcOver_mode;
    }
    if(mode == GBlendMode::kDstOver) {return DstOver_mode;}
    if(mode == GBlendMode::kSrcIn) {return SrcIn_mode;}
    if (mode == GBlendMode::kDstIn)
    {
        if (Src_alpha <= 0 && shader==nullptr) {return clear_mode;}
        if ((Src_alpha >= 1 && shader==nullptr) ||  (isOpaque && shader!=nullptr)) {return Dst_mode;}
        return DstIn_mode;
    }
    if(mode == GBlendMode::kSrcOut) {return SrcOut_mode;}
    if (mode == GBlendMode::kDstOut)
    {
        if (Src_alpha <= 0 && shader==nullptr) {return Dst_mode;}
        if ((Src_alpha >= 1 && shader==nullptr) ||  (isOpaque && shader!=nullptr)) {return clear_mode;}
        return DstOut_mode;
    }
    if (mode == GBlendMode::kSrcATop)
    {
        if (Src_alpha <= 0 && shader==nullptr) {return Dst_mode;}
        if ((Src_alpha >= 1 && shader==nullptr) ||  (isOpaque && shader!=nullptr)) {return SrcIn_mode;}
        return SrcATop_mode;
    }
    if (mode == GBlendMode::kDstATop)
    {
        if (Src_alpha <= 0 && shader==nullptr) {return clear_mode;}
        if ((Src_alpha >= 1 && shader==nullptr) ||  (isOpaque && shader!=nullptr)) {return DstOver_mode;}
        return DstATop_mode;
    }
    
    if (mode == GBlendMode::kXor)
    {
        if (Src_alpha <= 0 && shader==nullptr) {return Dst_mode;}
        if ((Src_alpha >= 1 && shader==nullptr) ||  (isOpaque && shader!=nullptr)) {return SrcOut_mode;}
        return Xor_mode;
    }

    return clear_mode;
}

// GPixel calc_blend_mode(GPixel dst, GPaint src, MyColor s_premul)
// {

//     switch (src.getBlendMode())
//     {
//     case GBlendMode::kClear:
//     {
//         return GPixel_PackARGB(0, 0, 0, 0);
//     }
//     case GBlendMode::kSrc:
//     {
//         // pin 0-1
//         if (s_premul.fA <= 0)
//         {
//             return GPixel_PackARGB(0, 0, 0, 0);
//         }
//         else
//         {
//             return GPixel_PackARGB(s_premul.fA, s_premul.fR, s_premul.fG, s_premul.fB);
//         }
//         break;
//     }
//     case GBlendMode::kDst:
//     {
//         return dst;
//     }
//     case GBlendMode::kSrcOver:
//     { //S + (1 - Sa) * D

//         int S_alpha = 255 - s_premul.fA;
//         if (S_alpha >= 255)
//         {
//             return dst;
//         }
//         else if (S_alpha <= 0)
//         {
//             return GPixel_PackARGB(s_premul.fA, s_premul.fR, s_premul.fG, s_premul.fB);
//         }
//         else
//         {
//             return GPixel_PackARGB((s_premul.fA + Div255Shift(S_alpha * GPixel_GetA(dst))),
//                                    (s_premul.fR + Div255Shift(S_alpha * GPixel_GetR(dst))),
//                                    (s_premul.fG + Div255Shift(S_alpha * GPixel_GetG(dst))),
//                                    (s_premul.fB + Div255Shift(S_alpha * GPixel_GetB(dst))));
//         }
//         break;
//     }
//     case GBlendMode::kDstOver:
//     { //!<     D + (1 - Da)*S
//         int Da = GPixel_GetA(dst);
//         int D_alpha = 255 - Da;
//         if (Da <= 0)
//         {
//             return GPixel_PackARGB(s_premul.fA, s_premul.fR, s_premul.fG, s_premul.fB);
//         }
//         else if (Da >= 255)
//         {
//             return dst;
//         }
//         else
//         {
//             return GPixel_PackARGB((GPixel_GetA(dst) + Div255Shift(D_alpha * s_premul.fA)),
//                                    (GPixel_GetR(dst) + Div255Shift(D_alpha * s_premul.fR)),
//                                    (GPixel_GetG(dst) + Div255Shift(D_alpha * s_premul.fG)),
//                                    (GPixel_GetB(dst) + Div255Shift(D_alpha * s_premul.fB)));
//         }
//         break;
//     }
//     case GBlendMode::kSrcIn:
//     { //!<     Da * S
//         int Da = GPixel_GetA(dst);
//         if (Da <= 0)
//         {
//             return GPixel_PackARGB(0, 0, 0, 0);
//         }
//         // premul

//         if (Da >= 255)
//         {
//             return GPixel_PackARGB(s_premul.fA, s_premul.fR, s_premul.fG, s_premul.fB);
//         }
//         else
//         {
//             return GPixel_PackARGB(Div255Shift(Da * s_premul.fA), Div255Shift(Da * s_premul.fR), Div255Shift(Da * s_premul.fG), Div255Shift(Da * s_premul.fB));
//         }
//         break;
//     }
//     case GBlendMode::kDstIn:
//     { //!<     Sa * D

//         int Sa = s_premul.fA;
//         if (Sa <= 0)
//         {
//             return GPixel_PackARGB(0, 0, 0, 0);
//         }
//         else if (Sa >= 255)
//         {
//             return dst;
//         }

//         // premul
//         return GPixel_PackARGB(Div255Shift(Sa * GPixel_GetA(dst)),
//                                Div255Shift(Sa * GPixel_GetR(dst)),
//                                Div255Shift(Sa * GPixel_GetG(dst)),
//                                Div255Shift(Sa * GPixel_GetB(dst)));
//     }
//     case GBlendMode::kSrcOut:
//     { //!<     (1 - Da)*S
//         int Da = GPixel_GetA(dst);
//         int D_alpha = 255 - Da;
//         if (Da <= 0)
//         {
//             return GPixel_PackARGB(s_premul.fA, s_premul.fR, s_premul.fG, s_premul.fB);
//         }
//         else if (Da >= 255)
//         {
//             return GPixel_PackARGB(0, 0, 0, 0);
//         }
//         else
//         {
//             return GPixel_PackARGB(Div255Shift(D_alpha * s_premul.fA),
//                                    Div255Shift(D_alpha * s_premul.fR),
//                                    Div255Shift(D_alpha * s_premul.fG),
//                                    Div255Shift(D_alpha * s_premul.fB));
//         }
//         break;
//     }
//     case GBlendMode::kDstOut:
//     { //!<     (1 - Sa)*D
//         int S_alpha = 255 - s_premul.fA;
//         if (S_alpha >= 255)
//         {

//             return dst;
//         }
//         else if (S_alpha <= 0)
//         {
//             return GPixel_PackARGB(0, 0, 0, 0);
//         }
//         else
//         {

//             return GPixel_PackARGB(Div255Shift(S_alpha * GPixel_GetA(dst)),
//                                    Div255Shift(S_alpha * GPixel_GetR(dst)),
//                                    Div255Shift(S_alpha * GPixel_GetG(dst)),
//                                    Div255Shift(S_alpha * GPixel_GetB(dst)));
//         }
//         break;
//     }
//     case GBlendMode::kSrcATop:
//     { //!<     Da*S + (1 - Sa)*D
//         int Da = GPixel_GetA(dst);
//         int S_alpha = 255 - s_premul.fA;
//         if (S_alpha >= 255)
//         {
//             return dst;
//         }

//         if (S_alpha <= 0)
//         {
//             if (Da <= 0)
//             {
//                 return GPixel_PackARGB(0, 0, 0, 0);
//             }
//             return GPixel_PackARGB(Div255Shift(Da * s_premul.fA), Div255Shift(Da * s_premul.fR), Div255Shift(Da * s_premul.fG), Div255Shift(Da * s_premul.fB));
//         }
//         else
//         {
//             return GPixel_PackARGB(Div255Shift(Da * s_premul.fA + S_alpha * GPixel_GetA(dst)),
//                                    Div255Shift(Da * s_premul.fR + S_alpha * GPixel_GetR(dst)),
//                                    Div255Shift(Da * s_premul.fG + S_alpha * GPixel_GetG(dst)),
//                                    Div255Shift(Da * s_premul.fB + S_alpha * GPixel_GetB(dst)));
//         }
//         break;
//     }
//     case GBlendMode::kDstATop:
//     { //!<     Sa*D + (1 - Da)*S
//         int Sa = s_premul.fA;
//         int Da = GPixel_GetA(dst);
//         int D_alpha = 255 - Da;
//         if (Da <= 0)
//         {
//             return GPixel_PackARGB(s_premul.fA, s_premul.fR, s_premul.fG, s_premul.fB);
//         }
//         if (Da >= 255)
//         {
//             if (Sa <= 0)
//             {
//                 return GPixel_PackARGB(0, 0, 0, 0);
//             }

//             return GPixel_PackARGB(Div255Shift(Sa * GPixel_GetA(dst)),
//                                    Div255Shift(Sa * GPixel_GetR(dst)),
//                                    Div255Shift(Sa * GPixel_GetG(dst)),
//                                    Div255Shift(Sa * GPixel_GetB(dst)));
//         } // Sa*D + (1 - Da)*S
//         else
//         {
//             return GPixel_PackARGB(Div255Shift(D_alpha * s_premul.fA + Sa * GPixel_GetA(dst)),
//                                    Div255Shift(D_alpha * s_premul.fR + Sa * GPixel_GetR(dst)),
//                                    Div255Shift(D_alpha * s_premul.fG + Sa * GPixel_GetG(dst)),
//                                    Div255Shift(D_alpha * s_premul.fB + Sa * GPixel_GetB(dst)));
//         }
//         break;
//     }
//     case GBlendMode::kXor:
//     { //!<     (1 - Sa)*D + (1 - Da)*S
//         int Sa = s_premul.fA;
//         int Da = GPixel_GetA(dst);
//         int S_alpha = 255 - Sa;
//         int D_alpha = 255 - Da;

//         if (Sa <= 0)
//         {
//             return dst;
//         }
//         if (Da <= 0)
//         {
//             return GPixel_PackARGB(s_premul.fA, s_premul.fR, s_premul.fG, s_premul.fB);
//         }
//         if (Sa >= 255)
//         {
//             return GPixel_PackARGB(Div255Shift(D_alpha * s_premul.fA),
//                                    Div255Shift(D_alpha * s_premul.fR),
//                                    Div255Shift(D_alpha * s_premul.fG),
//                                    Div255Shift(D_alpha * s_premul.fB));
//         }
//         if (Da >= 255)
//         {
//             return GPixel_PackARGB(Div255Shift(S_alpha * GPixel_GetA(dst)),
//                                    Div255Shift(S_alpha * GPixel_GetR(dst)),
//                                    Div255Shift(S_alpha * GPixel_GetG(dst)),
//                                    Div255Shift(S_alpha * GPixel_GetB(dst)));
//         }
//         else
//         { //(1 - Sa)*D + (1 - Da)*S

//             return GPixel_PackARGB(Div255Shift(D_alpha * s_premul.fA + S_alpha * GPixel_GetA(dst)),
//                                    Div255Shift(D_alpha * s_premul.fR + S_alpha * GPixel_GetR(dst)),
//                                    Div255Shift(D_alpha * s_premul.fG + S_alpha * GPixel_GetG(dst)),
//                                    Div255Shift(D_alpha * s_premul.fB + S_alpha * GPixel_GetB(dst)));
//         }
//         break;
//     }
//     }
//     return GPixel_PackARGB(0, 0, 0, 0);
// }
