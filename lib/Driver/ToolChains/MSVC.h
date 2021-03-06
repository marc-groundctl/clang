//===--- MSVC.h - MSVC ToolChain Implementations ----------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_MSVC_H
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_MSVC_H

#include "Cuda.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace tools {

/// Visual studio tools.
namespace visualstudio {
class LLVM_LIBRARY_VISIBILITY Linker : public Tool {
public:
  Linker(const ToolChain &TC)
      : Tool("visualstudio::Linker", "linker", TC, RF_Full,
             llvm::sys::WEM_UTF16) {}

  bool hasIntegratedCPP() const override { return false; }
  bool isLinkJob() const override { return true; }

  void ConstructJob(Compilation &C, const JobAction &JA,
                    const InputInfo &Output, const InputInfoList &Inputs,
                    const llvm::opt::ArgList &TCArgs,
                    const char *LinkingOutput) const override;
};

class LLVM_LIBRARY_VISIBILITY Compiler : public Tool {
public:
  Compiler(const ToolChain &TC)
      : Tool("visualstudio::Compiler", "compiler", TC, RF_Full,
             llvm::sys::WEM_UTF16) {}

  bool hasIntegratedAssembler() const override { return true; }
  bool hasIntegratedCPP() const override { return true; }
  bool isLinkJob() const override { return false; }

  void ConstructJob(Compilation &C, const JobAction &JA,
                    const InputInfo &Output, const InputInfoList &Inputs,
                    const llvm::opt::ArgList &TCArgs,
                    const char *LinkingOutput) const override;

  std::unique_ptr<Command> GetCommand(Compilation &C, const JobAction &JA,
                                      const InputInfo &Output,
                                      const InputInfoList &Inputs,
                                      const llvm::opt::ArgList &TCArgs,
                                      const char *LinkingOutput) const;
};
} // end namespace visualstudio

} // end namespace tools

namespace toolchains {

class LLVM_LIBRARY_VISIBILITY MSVCToolChain : public ToolChain {
public:
  MSVCToolChain(const Driver &D, const llvm::Triple &Triple,
                const llvm::opt::ArgList &Args);

  llvm::opt::DerivedArgList *
  TranslateArgs(const llvm::opt::DerivedArgList &Args, StringRef BoundArch,
                Action::OffloadKind DeviceOffloadKind) const override;

  bool IsIntegratedAssemblerDefault() const override;
  bool IsUnwindTablesDefault() const override;
  bool isPICDefault() const override;
  bool isPIEDefault() const override;
  bool isPICDefaultForced() const override;

  void
  AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
                            llvm::opt::ArgStringList &CC1Args) const override;
  void AddClangCXXStdlibIncludeArgs(
      const llvm::opt::ArgList &DriverArgs,
      llvm::opt::ArgStringList &CC1Args) const override;

  void AddCudaIncludeArgs(const llvm::opt::ArgList &DriverArgs,
                          llvm::opt::ArgStringList &CC1Args) const override;

  bool getWindowsSDKDir(std::string &path, int &major,
                        std::string &windowsSDKIncludeVersion,
                        std::string &windowsSDKLibVersion) const;
  bool getWindowsSDKLibraryPath(std::string &path) const;
  /// \brief Check if Universal CRT should be used if available
  bool useUniversalCRT(std::string &visualStudioDir) const;
  bool getUniversalCRTSdkDir(std::string &path, std::string &ucrtVersion) const;
  bool getUniversalCRTLibraryPath(std::string &path) const;
  bool getVisualStudioInstallDir(std::string &path) const;
  bool getVisualStudioBinariesFolder(const char *clangProgramPath,
                                     std::string &path) const;
  VersionTuple
  computeMSVCVersion(const Driver *D,
                     const llvm::opt::ArgList &Args) const override;

  std::string ComputeEffectiveClangTriple(const llvm::opt::ArgList &Args,
                                          types::ID InputType) const override;
  SanitizerMask getSupportedSanitizers() const override;

  void printVerboseInfo(raw_ostream &OS) const override;

protected:
  void AddSystemIncludeWithSubfolder(const llvm::opt::ArgList &DriverArgs,
                                     llvm::opt::ArgStringList &CC1Args,
                                     const std::string &folder,
                                     const Twine &subfolder1,
                                     const Twine &subfolder2 = "",
                                     const Twine &subfolder3 = "") const;

  Tool *buildLinker() const override;
  Tool *buildAssembler() const override;
private:
  VersionTuple getMSVCVersionFromTriple() const;
  VersionTuple getMSVCVersionFromExe() const;

  CudaInstallationDetector CudaInstallation;
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif // LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_MSVC_H
