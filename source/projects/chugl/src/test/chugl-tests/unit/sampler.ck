/*
Tests for TextureSampler
*/

TextureSampler defaultSampler;

T.assert(
    TextureSampler.Wrap_Repeat == 0
    &&
    TextureSampler.Wrap_Mirror == 1
    &&
    TextureSampler.Wrap_Clamp == 2
    &&
    TextureSampler.Filter_Nearest == 0
    &&
    TextureSampler.Filter_Linear == 1,
    "TextureSampler constants incorrect"
);

T.assert(
    defaultSampler.wrapU == TextureSampler.Wrap_Repeat
    &&
    defaultSampler.wrapV == TextureSampler.Wrap_Repeat
    &&
    defaultSampler.wrapW == TextureSampler.Wrap_Repeat
    &&
    defaultSampler.filterMin == TextureSampler.Filter_Linear
    &&
    defaultSampler.filterMag == TextureSampler.Filter_Linear
    &&
    defaultSampler.filterMip == TextureSampler.Filter_Linear,
    "defaultSampler values incorrect"
);

/* 
Texture tests
*/

