#include "fresnel.hpp"
#include "microfacet.hpp"
#include <cmath>
#include <lightwave.hpp>
#include <vector>

namespace lightwave {

class LayeredMatrix : public Bsdf {
    ref<Texture> m_roughness;
    ref<Texture> m_inroughness;
    ref<Texture> m_baseColor;

    int N; // Number of discretized angles
    std::vector<std::vector<float>> R_t, R_b, T_tb, T_bt;

public:
    LayeredMatrix(const Properties &properties) {
        N = 4; // Discretize angles for matrix representation

        m_roughness   = properties.get<Texture>("roughness");
        m_inroughness = properties.get<Texture>("inroughness");
        m_baseColor   = properties.get<Texture>("baseColor");
        // R_t         = properties.get<Matrix4x4>("Rt");
        // R_b         = properties.get<Matrix4x4>("Rb");
        // T_tb        = properties.get<Matrix4x4>("Ttb");
        // T_bt        = properties.get<Matrix4x4>("Tbt");

        // Put your hardcoded matrices here :D
        // R_t = {
        //     { 0.6, 0.1, 0.1, 0.1 },
        //     { 0.1, 0.6, 0.1, 0.1 },
        //     { 0.1, 0.1, 0.6, 0.1 },
        //     { 0.1, 0.1, 0.1, 0.6 },
        // };
        // R_b = {
        //     { 0.93, 0.93, 0.93, 0.93 },
        //     { 0.93, 0.94, 0.93, 0.93 },
        //     { 0.93, 0.93, 0.93, 0.93 },
        //     { 0.93, 0.93, 0.93, 0.93 },
        // };
        // T_tb = {
        //     { 0.2, 0.2, 0.2, 0.19 },
        //     { 0.2, 0.2, 0.2, 0.19 },
        //     { 0.2, 0.2, 0.2, 0.19 },
        //     { 0.2, 0.2, 0.2, 0.19 },
        // };
        // T_bt = {
        //     { 0.07, 0.07, 0.07, 0.07 },
        //     { 0.07, 0.06, 0.07, 0.07 },
        //     { 0.07, 0.07, 0.07, 0.07 },
        //     { 0.07, 0.07, 0.07, 0.07 },
        // };

        // R_t = {
        //     { 0.94, 0.94, 0.94, 0.93 },
        //     { 0.94, 0.94, 0.94, 0.93 },
        //     { 0.94, 0.94, 0.94, 0.93 },
        //     { 0.94, 0.94, 0.94, 0.94 },
        // };
        // R_b = {
        //     { 0.5, 0.1, 0.1, 0.1 },
        //     { 0.1, 0.5, 0.1, 0.1 },
        //     { 0.1, 0.1, 0.5, 0.1 },
        //     { 0.1, 0.1, 0.1, 0.5 },
        // };
        // T_tb = {
        //     { 0.06, 0.06, 0.06, 0.07 },
        //     { 0.06, 0.06, 0.06, 0.07 },
        //     { 0.06, 0.06, 0.06, 0.07 },
        //     { 0.06, 0.06, 0.06, 0.06 },
        // };
        // T_bt = {
        //     { 0.22, 0.21, 0.21, 0.19 },
        //     { 0.22, 0.21, 0.21, 0.19 },
        //     { 0.22, 0.21, 0.21, 0.19 },
        //     { 0.22, 0.21, 0.21, 0.19 },
        // };

        // R_t = {
        //     { 0.05, 0.02, 0.01, 0.0 },
        //     { 0.02, 0.1, 0.03, 0.01 },
        //     { 0.01, 0.03, 0.15, 0.04 },
        //     { 0.0, 0.01, 0.04, 0.2 },
        // };
        // R_b = {
        //     { 0.06, 0.03, 0.02, 0.01 },
        //     { 0.03, 0.12, 0.04, 0.02 },
        //     { 0.02, 0.04, 0.18, 0.05 },
        //     { 0.01, 0.02, 0.05, 0.22 },
        // };
        // T_tb = {
        //     { 0.94, 0.8, 0.8, 0.8 },
        //     { 0.8, 0.95, 0.85, 0.8 },
        //     { 0.8, 0.85, 0.9, 0.8 },
        //     { 0.9, 0.85, 0.8, 0.85 },
        // };
        // T_bt = {
        //     { 0.98, 0.88, 0.78, 0.68 },
        //     { 0.88, 0.93, 0.83, 0.73 },
        //     { 0.78, 0.83, 0.88, 0.78 },
        //     { 0.78, 0.73, 0.78, 0.83 },
        // };

        // R_t = {
        //     { 0.7, 0.15, 0.1, 0.05 },
        //     { 0.15, 0.7, 0.1, 0.05 },
        //     { 0.1, 0.1, 0.7, 0.1 },
        //     { 0.05, 0.05, 0.1, 0.8 },
        // };
        // R_b = {
        //     { 0.65, 0.1, 0.15, 0.1 },
        //     { 0.1, 0.65, 0.15, 0.1 },
        //     { 0.15, 0.15, 0.65, 0.05 },
        //     { 0.1, 0.1, 0.05, 0.7 },
        // };
        // T_tb = {
        //     { 0.05, 0.02, 0.01, 0.01 },
        //     { 0.02, 0.05, 0.01, 0.01 },
        //     { 0.01, 0.01, 0.05, 0.02 },
        //     { 0.01, 0.01, 0.02, 0.05 },
        // };
        // T_bt = {
        //     { 0.05, 0.02, 0.01, 0.01 },
        //     { 0.02, 0.05, 0.01, 0.01 },
        //     { 0.01, 0.01, 0.05, 0.02 },
        //     { 0.01, 0.01, 0.02, 0.05 },
        // };

        R_t = {
            { 0.97, 0.97, 0.97, 0.97 },
            { 0.97, 0.97, 0.97, 0.97 },
            { 0.97, 0.97, 0.97, 0.97 },
            { 0.97, 0.97, 0.97, 0.97 },
        };
        R_b = {
            { 0.65, 0.1, 0.15, 0.1 },
            { 0.1, 0.65, 0.15, 0.1 },
            { 0.15, 0.15, 0.65, 0.05 },
            { 0.1, 0.1, 0.05, 0.7 },
        };
        T_tb = {
            { 0.03, 0.03, 0.03, 0.03 },
            { 0.03, 0.03, 0.03, 0.03 },
            { 0.03, 0.03, 0.03, 0.03 },
            { 0.03, 0.03, 0.03, 0.03 },
        };
        T_bt = {
            { 0.1, 0.1, 0.1, 0.09 },
            { 0.1, 0.1, 0.1, 0.1 },
            { 0.09, 0.09, 0.1, 0.1 },
            { 0.09, 0.09, 0.1, 0.1 },
        };

        // R_t = {
        //     { 0.05, 0.02, 0.01, 0.0 },
        //     { 0.02, 0.1, 0.03, 0.01 },
        //     { 0.01, 0.03, 0.15, 0.04 },
        //     { 0.0, 0.01, 0.04, 0.2 },
        // };
        // R_b = {
        //     { 0.06, 0.03, 0.02, 0.01 },
        //     { 0.03, 0.12, 0.04, 0.02 },
        //     { 0.02, 0.04, 0.18, 0.05 },
        //     { 0.01, 0.02, 0.05, 0.22 },
        // };
        // T_tb = {
        //     { 0.94, 0.8, 0.8, 0.8 },
        //     { 0.8, 0.95, 0.85, 0.8 },
        //     { 0.8, 0.85, 0.9, 0.8 },
        //     { 0.9, 0.85, 0.8, 0.85 },
        // };
        // T_bt = {
        //     { 0.98, 0.88, 0.78, 0.68 },
        //     { 0.88, 0.93, 0.83, 0.73 },
        //     { 0.78, 0.83, 0.88, 0.78 },
        //     { 0.78, 0.73, 0.78, 0.83 },
        // };

        // Always build after putting your matrices
        //  Too lazy to modify parser Zzzz
    }

    int find_closest_angle_index(float cos_theta, int N) const {
        // Ensure the angle is between -1 and 1
        cos_theta = std::clamp(cos_theta, -1.0f, 1.0f);
        // Map the cosine of the angle to a discretized angle index
        // cos_theta ranges from -1 to 1, and we map this to the range 0 to N-1
        float angle_index = (cos_theta + 1.0f) / 2.0f * (N - 1);
        // Round the index to the nearest integer
        int closest_index = static_cast<int>(std::round(angle_index));
        // Ensure the index is within bounds (0 to N-1)
        return std::clamp(closest_index, 0, N - 1);
    }

    BsdfEval evaluate(const Point2 &uv, const Vector &wo,
                      const Vector &wi) const override {
        const auto alpha = std::max(float(1e-3), sqr(m_roughness->scalar(uv)));
        const auto inalpha =
            std::max(float(1e-3), sqr(m_inroughness->scalar(uv)));
        const auto baseColor = m_baseColor->evaluate(uv);

        Vector wh = (wi + wo).normalized();
        int i     = find_closest_angle_index(Frame::cosTheta(wi), N);
        int j     = find_closest_angle_index(Frame::cosTheta(wo), N);

        Color R_t_eval  = Color(R_t[i][j]);
        Color R_b_eval  = Color(R_b[i][j]);
        Color T_tb_eval = Color(T_tb[i][j]);
        Color T_bt_eval = Color(T_bt[i][j]);

        // Color Fr = R_t_eval;
        // Color Ft = T_tb_eval;

        float cosThetao = Frame::absCosTheta(wo);
        if (cosThetao == 0) {
            return BsdfEval(); // Invalid
        }

        float currentAlpha;
        Color R;
        Color T;
        if (Frame::cosTheta(wi) < 0) {
            currentAlpha = inalpha;
            R            = R_b_eval;
            T            = T_bt_eval;
        } else {
            currentAlpha = alpha;
            R            = R_t_eval;
            T            = T_tb_eval;
        }

        float D   = microfacet::evaluateGGX(currentAlpha, wh);
        float Gwi = microfacet::smithG1(currentAlpha, wh, wi);
        float Gwo = microfacet::smithG1(currentAlpha, wh, wo);

        // Compute reflection and transmission components
        Color Fr = (R * D * Gwi * Gwo) / (4 * cosThetao);
        // Transmission contribution
        Color Ft = (T * D * Gwi * Gwo) / (4 * cosThetao);

        Fr *= baseColor;
        Ft *= baseColor;

        // if (Frame::cosTheta(wo) < 0) {
        //     Fr += R_b_eval * Ft;
        //     Ft *= T_bt_eval;
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
        const auto baseColor = m_baseColor->evaluate(uv);

        Vector wh =
            microfacet::sampleGGXVNDF(alpha, wo, rng.next2D()).normalized();

        int i = find_closest_angle_index(Frame::cosTheta(wo), N);
        int j = floor(rng.next() * 4);

        Color R_t_eval  = Color(R_t[i][j]);
        Color R_b_eval  = Color(R_b[i][j]);
        Color T_tb_eval = Color(T_tb[i][j]);
        Color T_bt_eval = Color(T_bt[i][j]);

        float eta = 1.5f;
        // if (Frame::cosTheta(wo) < 0) {
        //     eta = 1 / eta;
        // }

        float reflectionProbability =
            fresnelDielectric(Frame::cosTheta(wo), eta);
        reflectionProbability = std::clamp(reflectionProbability, 0.3f, 0.9f);

        Vector wi;
        Color weight;

        if (rng.next() < reflectionProbability) {
            // wi     = reflect(wo, Vector(0, 0, 1)).normalized();
            // weight = R_t_eval / reflectionProbability;
            // wi        = reflect(wo, wh).normalized();

            if (alpha < 1e-2 or inalpha < 1e-2) {
                Vector n = (Frame::cosTheta(wi) > 0)
                               ? Vector(0, 0, 1)
                               : Vector(0, 0, -1); // Local shading normal
                wi       = reflect(wo, n).normalized();
            } else {
                wi = reflect(wo, wh).normalized();
            }

            float G;
            Color R;
            if (Frame::cosTheta(wi) < 0) {
                G = microfacet::smithG1(inalpha, wh, wi);
                R = R_b_eval;
            } else {
                G = microfacet::smithG1(alpha, wh, wi);
                R = R_t_eval;
            }
            weight = (R * G) / reflectionProbability;
        } else {
            // wi     = refract(wo, Vector(0, 0, 1), eta).normalized();
            // weight = T_tb_eval / (1.0f - reflectionProbability);

            wi = refract(wo, wh, eta).normalized();

            float G;
            Color T;

            if (Frame::cosTheta(wi) < 0) {
                G = microfacet::smithG1(inalpha, wh, wi);
                T = T_bt_eval;
            } else {
                G = microfacet::smithG1(alpha, wh, wi);
                T = T_tb_eval;
            }
            weight = (T * G) / (1.0f - reflectionProbability);

            // if (Frame::cosTheta(wi) < 0) {
            //     weight += R_b_eval * Gwi;
            // }
            // weight *= T_bt_eval;
        }

        weight *= baseColor;

        BsdfSample bsdfSample;
        bsdfSample.weight = weight;
        bsdfSample.wi     = wi;
        return bsdfSample;
    }

    std::string toString() const override {
        return "LayeredMatrix[N=" + std::to_string(N) + "]";
    }
};

} // namespace lightwave

REGISTER_BSDF(LayeredMatrix, "layeredmatrix")