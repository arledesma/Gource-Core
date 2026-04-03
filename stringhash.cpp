/*
    Copyright (c) 2009 Andrew Caudwell (acaudwell@gmail.com)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. The name of the author may not be used to endorse or promote products
       derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "stringhash.h"

#include <algorithm>

int gStringHashSeed = 31;
std::vector<vec3> gColourPalette;
float gColourSpread = 0.5f;

static float hueToRgb(float p, float q, float t) {
    if(t < 0.0f) t += 1.0f;
    if(t > 1.0f) t -= 1.0f;
    if(t < 1.0f/6.0f) return p + (q - p) * 6.0f * t;
    if(t < 1.0f/2.0f) return q;
    if(t < 2.0f/3.0f) return p + (q - p) * (2.0f/3.0f - t) * 6.0f;
    return p;
}

static vec3 rgbToHsl(const vec3& rgb) {
    float maxc = std::max({rgb.x, rgb.y, rgb.z});
    float minc = std::min({rgb.x, rgb.y, rgb.z});
    float l = (maxc + minc) * 0.5f;

    if(maxc == minc) return vec3(0.0f, 0.0f, l);

    float d = maxc - minc;
    float s = (l > 0.5f) ? d / (2.0f - maxc - minc) : d / (maxc + minc);

    float h;
    if(maxc == rgb.x) {
        h = (rgb.y - rgb.z) / d + (rgb.y < rgb.z ? 6.0f : 0.0f);
    } else if(maxc == rgb.y) {
        h = (rgb.z - rgb.x) / d + 2.0f;
    } else {
        h = (rgb.x - rgb.y) / d + 4.0f;
    }
    h /= 6.0f;

    return vec3(h, s, l);
}

static vec3 hslToRgb(const vec3& hsl) {
    if(hsl.y == 0.0f) return vec3(hsl.z, hsl.z, hsl.z);

    float q = (hsl.z < 0.5f) ? hsl.z * (1.0f + hsl.y) : hsl.z + hsl.y - hsl.z * hsl.y;
    float p = 2.0f * hsl.z - q;

    return vec3(
        hueToRgb(p, q, hsl.x + 1.0f/3.0f),
        hueToRgb(p, q, hsl.x),
        hueToRgb(p, q, hsl.x - 1.0f/3.0f)
    );
}

int stringHash(const std::string& str) {

    int val = 0;
    int n = str.size();

    for (int i = 0; i < n; i++) {
        val = val + str[i] * (gStringHashSeed^(n-i));
    }

     if(val<0) {
         val = -val;
     }

    return val;
}

vec2 vec2Hash(const std::string& str) {
    int hash = stringHash(str);

    int x = ((hash/7) % 255) - 127;
    int y = ((hash/3) % 255) - 127;

    vec2 v = normalise(vec2(x, y));

    return v;
}

vec3 vec3Hash(const std::string& str) {
    int hash = stringHash(str);

    int x = ((hash/7) % 255) - 127;
    int y = ((hash/3) % 255) - 127;
    int z = hash % 255;

    vec3 v = normalise(vec3(x, y, z));

    return v;
}

vec3 colourHash(const std::string& str) {
    int hash = stringHash(str);

    if(hash == 0) hash++;

    if(!gColourPalette.empty()) {
        vec3 base = gColourPalette[hash % gColourPalette.size()];
        vec3 hsl = rgbToHsl(base);

        float sShift = (((hash / 7) % 200) - 100) / 100.0f * gColourSpread * 0.4f;
        float lShift = (((hash / 3) % 200) - 100) / 100.0f * gColourSpread * 0.3f;

        hsl.y = std::min(std::max(hsl.y + sShift, 0.0f), 1.0f);
        hsl.z = std::min(std::max(hsl.z + lShift, 0.1f), 0.9f);

        return hslToRgb(hsl);
    }

    if(str.empty()) return vec3(1.0f, 1.0f, 1.0f);

    int r = (hash/7) % 255;
    int g = (hash/3) % 255;
    int b = hash % 255;

    vec3 colour = normalise(vec3(r, g, b));

    return colour;
}
