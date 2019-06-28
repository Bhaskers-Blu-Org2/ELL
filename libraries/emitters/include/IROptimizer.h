////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IROptimizer.h (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "LLVMUtilities.h"

#include <llvm/IR/LegacyPassManager.h>

namespace ell
{
namespace emitters
{
    class IRModuleEmitter;

    ///<summary> Class to manage LLVM optimizations </summary>
    class IROptimizer
    {
    public:
        /// <summary> Function optimizer for functions in this module. </summary>
        ///
        /// <param name="module"> The module. </param>
        IROptimizer(IRModuleEmitter& module);

        ~IROptimizer();

        /// <summary> Add common optimizations to the optimizer pipeline. </summary>
        void AddStandardPasses();

        /// <summary> Optimize the given function. </summary>
        ///
        /// <param name="pFunction"> pointer to the function to optimize. </param>
        void BeginOptimizeFunctions();
        void OptimizeFunction(LLVMFunction pFunction);
        void EndOptimizeFunctions();

        /// <summary> Optimize the module. </summary>
        void OptimizeModule(llvm::Module* pModule);

    private:
        IRModuleEmitter& _module;
        llvm::legacy::PassManager _modulePasses;
        llvm::legacy::FunctionPassManager _functionPasses;
    };
} // namespace emitters
} // namespace ell
