////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralLayersInterface.h (interfaces)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef SWIG

#include "MathInterface.h"

#include <math/include/Tensor.h>

#include <predictors/neural/include/BinaryConvolutionalLayer.h>
#include <predictors/neural/include/ConvolutionalLayer.h>
#include <predictors/neural/include/Layer.h>
#include <predictors/neural/include/PoolingLayer.h>
#include <predictors/neural/include/RegionDetectionLayer.h>

#include <array>
#include <string>
#include <vector>

#endif

// Helper to define a value that is exposed as read-only in the API, but is not
// const from the underlying code side.
// clang-format off
// clang-format breaks SWIG because clang-format likes to put a space after %
#ifdef SWIG
#define API_READONLY(statement) \
    %immutable;                 \
    statement;                  \
    %mutable
#else
#define API_READONLY(statement) \
    statement;
#endif
// clang-format on

namespace ell
{
namespace api
{
    namespace predictors
    {
        namespace neural
        {

            // Using clauses. These classes will be wrapped separately.
            using LayerType = ell::predictors::neural::LayerType;
            using PaddingScheme = ell::predictors::neural::PaddingScheme;
            using PaddingParameters = ell::predictors::neural::PaddingParameters;
            using LayerShape = ell::api::math::TensorShape;
            using DataType = ELL_API::PortType;

            //////////////////////////////////////////////////////////////////////////
            // Common types used by the neural layers
            //////////////////////////////////////////////////////////////////////////
            struct LayerParameters
            {
                LayerShape inputShape;
                PaddingParameters inputPaddingParameters;
                LayerShape outputShape;
                PaddingParameters outputPaddingParameters;
                DataType dataType;
            };

            //////////////////////////////////////////////////////////////////////////
            // Api classes for the neural layers
            //////////////////////////////////////////////////////////////////////////

            // Api projection for the layers base class
            class Layer
            {
            public:
                Layer(const LayerParameters& layerParameters) :
                    parameters(layerParameters) {}
                virtual ~Layer() {}

                virtual LayerType GetLayerType() const = 0;

                template <class LayerType>
                LayerType& As()
                {
                    auto ptr = dynamic_cast<LayerType*>(this);
                    if (!ptr)
                    {
                        throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch,
                                                        std::string("Expecting this layer to be of type : '") + typeid(LayerType).name() + "' but it is : '" + typeid(*this).name() + "'");
                    }
                    return *ptr;
                }

                template <class LayerType>
                bool Is()
                {
                    return dynamic_cast<LayerType*>(this) != nullptr;
                }

                const LayerParameters parameters;
            };

            // Api projections for ActivationLayer
            enum class ActivationType : int
            {
                relu,
                leaky,
                sigmoid,
                tanh,
                prelu,
                hardSigmoid
            };

            class ActivationLayer : public Layer
            {
            public:
                ActivationLayer(const LayerParameters& layerParameters, ActivationType activation) :
                    Layer(layerParameters),
                    activation(activation)
                {
                }

                LayerType GetLayerType() const override { return LayerType::activation; }

                const ActivationType activation;

#ifndef SWIG
                template <typename ElementType>
                static ell::predictors::neural::Activation<ElementType> CreateActivation(ActivationType activation);
#endif
            };

            // Api projection for PReLUActivationLayer
            class PReLUActivationLayer : public ActivationLayer
            {
            public:
                PReLUActivationLayer(const LayerParameters& layerParameters, const ell::api::math::Tensor<double>& alphaTensor) :
                    ActivationLayer(layerParameters, ActivationType::prelu),
                    alpha(alphaTensor)
                {
                }

                API_READONLY(ell::api::math::Tensor<double> alpha);
            };

            // Api projection for LeakyReLUActivationLayer
            class LeakyReLUActivationLayer : public ActivationLayer
            {
            public:
                LeakyReLUActivationLayer(const LayerParameters& layerParameters, double alpha) :
                    ActivationLayer(layerParameters, ActivationType::leaky),
                    _alpha(alpha)
                {
                }

                API_READONLY(double _alpha);
            };

            // Api projection for BatchNormalizationLayer
            enum class EpsilonSummand : int
            {
                variance,
                sqrtVariance
            };

            class BatchNormalizationLayer : public Layer
            {
            public:
                BatchNormalizationLayer(const LayerParameters& layerParameters, const std::vector<double>& mean, const std::vector<double>& variance, double epsilon, EpsilonSummand epsilonSummand) :
                    Layer(layerParameters),
                    mean(mean),
                    variance(variance),
                    epsilon(epsilon),
                    epsilonSummand(epsilonSummand)
                {
                }

                LayerType GetLayerType() const override { return LayerType::batchNormalization; }

                const std::vector<double> mean;
                const std::vector<double> variance;
                const double epsilon;
                const EpsilonSummand epsilonSummand;
            };

            // Api projection for BiasLayer
            class BiasLayer : public Layer
            {
            public:
                BiasLayer(const LayerParameters& layerParameters, const std::vector<double>& bias) :
                    Layer(layerParameters),
                    bias(bias)
                {
                }

                LayerType GetLayerType() const override { return LayerType::bias; }

                const std::vector<double> bias;
            };

            // Api projections for BinaryConvolutionalLayer
            using BinaryConvolutionMethod = ell::predictors::neural::BinaryConvolutionMethod;
            using BinaryConvolutionalParameters = ell::predictors::neural::BinaryConvolutionalParameters;

            class BinaryConvolutionalLayer : public Layer
            {
            public:
                BinaryConvolutionalLayer(const LayerParameters& layerParameters, const BinaryConvolutionalParameters& convolutionalParameters, const ell::api::math::Tensor<double>& weightsTensor) :
                    Layer(layerParameters),
                    weights(weightsTensor.data, weightsTensor.shape.rows, weightsTensor.shape.columns, weightsTensor.shape.channels),
                    convolutionalParameters(convolutionalParameters)
                {
                }

                LayerType GetLayerType() const override { return LayerType::binaryConvolution; }

                API_READONLY(ell::api::math::Tensor<double> weights);
                const BinaryConvolutionalParameters convolutionalParameters;
            };

            // Api projections for ConvolutionalLayer
            using ConvolutionMethod = ell::predictors::neural::ConvolutionMethod;
            using ConvolutionalParameters = ell::predictors::neural::ConvolutionalParameters;

            class ConvolutionalLayer : public Layer
            {
            public:
                ConvolutionalLayer(const LayerParameters& layerParameters, const ConvolutionalParameters& convolutionalParameters, const ell::api::math::Tensor<double>& weightsTensor) :
                    Layer(layerParameters),
                    weights(weightsTensor.data, weightsTensor.shape.rows, weightsTensor.shape.columns, weightsTensor.shape.channels),
                    convolutionalParameters(convolutionalParameters)
                {
                }

                LayerType GetLayerType() const override { return LayerType::convolution; }

                API_READONLY(ell::api::math::Tensor<double> weights);
                const ConvolutionalParameters convolutionalParameters;
            };

            // Api projections for FullyConnectedLayer
            class FullyConnectedLayer : public Layer
            {
            public:
                FullyConnectedLayer(const LayerParameters& layerParameters, const ell::api::math::Tensor<double>& weightsTensor) :
                    Layer(layerParameters),
                    weights(weightsTensor.data, weightsTensor.shape.rows, weightsTensor.shape.columns, weightsTensor.shape.channels)
                {
                }

                LayerType GetLayerType() const override { return LayerType::fullyConnected; }

                API_READONLY(ell::api::math::Tensor<double> weights);
            };

            // Api projections for PoolingLayer
            using PoolingParameters = ell::predictors::neural::PoolingParameters;

            enum class PoolingType : int
            {
                max,
                mean,
            };

            class PoolingLayer : public Layer
            {
            public:
                PoolingLayer(const LayerParameters& layerParameters, const PoolingParameters& poolingParameters, PoolingType poolingType) :
                    Layer(layerParameters),
                    poolingType(poolingType),
                    poolingParameters(poolingParameters)
                {
                }

                LayerType GetLayerType() const override { return LayerType::pooling; }

                const PoolingType poolingType;
                const PoolingParameters poolingParameters;
            };

            // Api projection for RegionDetectionLayer
            using RegionDetectionParameters = ell::predictors::neural::RegionDetectionParameters;

            class RegionDetectionLayer : public Layer
            {
            public:
                RegionDetectionLayer(const LayerParameters& layerParameters, const RegionDetectionParameters& detectionParameters) :
                    Layer(layerParameters),
                    detectionParameters(detectionParameters)
                {
                }

                LayerType GetLayerType() const override { return LayerType::region; }

                const RegionDetectionParameters detectionParameters;
            };

            // Api projection for SoftmaxLayer
            class SoftmaxLayer : public Layer
            {
            public:
                SoftmaxLayer(const LayerParameters& layerParameters) :
                    Layer(layerParameters)
                {
                }

                LayerType GetLayerType() const override { return LayerType::softmax; }
            };

            // Api projection for ScalingLayer
            class ScalingLayer : public Layer
            {
            public:
                ScalingLayer(const LayerParameters& layerParameters, const std::vector<double>& scales) :
                    Layer(layerParameters),
                    scales(scales)
                {
                }

                LayerType GetLayerType() const override { return LayerType::scaling; }

                const std::vector<double> scales;
            };
        } // namespace neural
    } // namespace predictors
} // namespace api
} // namespace ell
