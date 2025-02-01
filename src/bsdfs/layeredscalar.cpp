#include "fresnel.hpp"
#include "microfacet.hpp"
#include <lightwave.hpp>

namespace lightwave {

class LayeredScalar : public Bsdf {
    ref<Texture> m_roughness;
    ref<Texture> m_inroughness;
    ref<Texture> m_R11;
    ref<Texture> m_R22;
    ref<Texture> m_T12;
    ref<Texture> m_T21;

public:
    LayeredScalar(const Properties &properties) {
        m_roughness   = properties.get<Texture>("roughness");
        m_inroughness = properties.get<Texture>("inroughness");
        m_R11         = properties.get<Texture>("R11");
        m_R22         = properties.get<Texture>("R22");
        m_T12         = properties.get<Texture>("T12");
        m_T21         = properties.get<Texture>("T21");
    }

    BsdfEval evaluate(const Point2 &uv, const Vector &wo,
                      const Vector &wi) const override {
        const auto alpha = std::max(float(1e-3), sqr(m_roughness->scalar(uv)));
        const auto inalpha =
            std::max(float(1e-3), sqr(m_inroughness->scalar(uv)));

        Vector wh = (wi + wo).normalized();

        Color R11 = m_R11->evaluate(uv);
        Color R22 = m_R22->evaluate(uv);
        Color T12 = m_T12->evaluate(uv);
        Color T21 = m_T21->evaluate(uv);

        float currentAlpha;
        Color R;
        Color T;
        if (Frame::cosTheta(wi) < 0) {
            currentAlpha = inalpha;
            R            = R22;
            T            = T21;
        } else {
            currentAlpha = alpha;
            R            = R11;
            T            = T12;
        }

        // float currentAlpha =
        //     (Frame::cosTheta(wi) > 0 && Frame::cosTheta(wo) > 0) ? alpha
        //                                                          : inalpha;
        // Color R =
        //     (Frame::cosTheta(wi) > 0 && Frame::cosTheta(wo) > 0) ? R11 : R22;
        // Color T =
        //     (Frame::cosTheta(wi) > 0 && Frame::cosTheta(wo) > 0) ? T12 : T21;

        float D   = microfacet::evaluateGGX(currentAlpha, wh);
        float Gwi = microfacet::smithG1(currentAlpha, wh, wi);
        float Gwo = microfacet::smithG1(currentAlpha, wh, wo);

        float cosThetao = Frame::absCosTheta(wo);
        if (cosThetao == 0) {
            return BsdfEval(); // Invalid
        }

        // Compute reflection and transmission components

        Color Fr = (R * D * Gwi * Gwo) / (4 * cosThetao);
        Color Ft = (T * D * Gwi * Gwo) / (4 * cosThetao);

        // // If entering the material, add the back layer reflection
        // if (Frame::cosTheta(wo) < 0) {
        //     Fr += R22 *
        //           Ft;  // Light reflected from the back contributes to the
        //           BSDF
        //     Ft *= T21; // Transmission through the material
        // }

        BsdfEval bsdf;
        bsdf.value = Fr + Ft;
        return bsdf;
    }

    BsdfSample sample(const Point2 &uv, const Vector &wo,
                      Sampler &rng) const override {

        const auto alpha = std::max(float(1e-3), sqr(m_roughness->scalar(uv)));
        const auto inalpha =
            std::max(float(1e-3), sqr(m_inroughness->scalar(uv)));

        Vector wh =
            microfacet::sampleGGXVNDF(alpha, wo, rng.next2D()).normalized();

        Color R11 = m_R11->evaluate(uv);
        Color R22 = m_R22->evaluate(uv);
        Color T12 = m_T12->evaluate(uv);
        Color T21 = m_T21->evaluate(uv);

        // Reflection or transmission?
        // if (alpha < 1e-2) {
        //     reflectionProbability =
        //         1.0f; // Perfect mirror should always reflect
        // } else {
        //     reflectionProbability =
        //         fresnelDielectric(Frame::cosTheta(wo), 1.5f);
        // }
        float reflectionProbability =
            fresnelDielectric(Frame::cosTheta(wo), 1.5f);
        reflectionProbability = std::clamp(reflectionProbability, 0.2f, 0.95f);
        Vector wi;
        Color weight;

        if (rng.next() < reflectionProbability) {
            // Reflect
            if (alpha < 1e-2 or inalpha < 1e-2) {
                Vector n = (Frame::cosTheta(wi) > 0)
                               ? Vector(0, 0, 1)
                               : Vector(0, 0, -1); // Local shading normal
                wi       = reflect(wo, n).normalized();
            } else {
                wi = reflect(wo, wh).normalized();
            }
            // float Gwi   = microfacet::smithG1(alpha, wh, wi);
            // float inGwi = microfacet::smithG1(inalpha, wh, wi);
            float G;
            Color R;
            if (Frame::cosTheta(wi) < 0) {
                G = microfacet::smithG1(inalpha, wh, wi);
                R = R22;
            } else {
                G = microfacet::smithG1(alpha, wh, wi);
                R = R11;
            }

            // float G = (Frame::cosTheta(wi) > 0)
            //               ? microfacet::smithG1(alpha, wh, wi)
            //               : microfacet::smithG1(inalpha, wh, wi);

            // Select the correct reflection factor
            // Color R = (Frame::cosTheta(wi) > 0) ? R11 : R22;

            weight = (R * G) / reflectionProbability;

        } else {
            // Transmit
            float eta = 1.5f; // Assuming a fixed IOR
            wi        = refract(wo, wh, eta).normalized();
            // float Gwi   = microfacet::smithG1(alpha, wh, wi);
            // float inGwi = microfacet::smithG1(inalpha, wh, wi);
            float G;
            Color T;
            // float G = (Frame::cosTheta(wi) > 0)
            //               ? microfacet::smithG1(alpha, wh, wi)
            //               : microfacet::smithG1(inalpha, wh, wi);

            // // float G = (Frame::cosTheta(wo) > 0) ? Gwi : inGwi;
            // //  Select the correct transmission factor
            // Color T = (Frame::cosTheta(wi) > 0) ? T12 : T21;

            if (Frame::cosTheta(wi) < 0) {
                G = microfacet::smithG1(inalpha, wh, wi);
                T = T21;
            } else {
                G = microfacet::smithG1(alpha, wh, wi);
                T = T12;
            }

            weight = (T * G) / (1.0f - reflectionProbability);

            // // Handle internal reflection (R22) properly
            // if (Frame::cosTheta(wo) < 0 && Frame::cosTheta(wi) < 0) {
            //     // Light is still inside after refraction → apply R22
            //     weight *= R22 * inGwi;
            // }
        }

        BsdfSample bsdfSample;
        bsdfSample.weight = weight;
        bsdfSample.wi     = wi;
        return bsdfSample;
    }

    std::string toString() const override {
        return tfm::format(
            "LayeredScalar[\n"
            "  roughness = %s,\n"
            "  R11 = %s,\n"
            "  R22 = %s,\n"
            "  T12 = %s,\n"
            "  T21 = %s\n"
            "]",
            indent(m_roughness),
            indent(m_R11),
            indent(m_R22),
            indent(m_T12),
            indent(m_T21));
    }
};

} // namespace lightwave

REGISTER_BSDF(LayeredScalar, "layeredscalar")