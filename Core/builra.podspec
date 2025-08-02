def self.infer_version_from_git
  return Nothing unless Dir.exist?('.git')

  `git tag --merged HEAD --sort=committerdate`.split.select do |tag|
    Version.correct?(tag)
  end.last
end

Pod::Spec.new do |s|
  s.name         = "builra"
  s.version      = infer_version_from_git || (raise Informative, 'Could not infer `builra` version from git')
  s.summary      = "A low-level build system."

  s.description  = <<-DESC.strip_heredoc
                    **builra** is a set of libraries for building build systems. Unlike most build
                    system projects which focus on the syntax for describing the build, builra is
                    designed around a reusable, flexible, and scalable general purpose *build
                    engine* capable of solving many "build system"-like problems. The project also
                    includes additional libraries on top of that engine which provide support for
                    constructing *bespoke* build systems (like `swift build`) or for building from
                    Ninja manifests.
                   DESC

  s.homepage     = "https://github.com/apple/swift-builra"
  s.license      = { type: 'Apache 2.0', file: "LICENSE.txt" }

  s.documentation_url  = "https://builra.readthedocs.io/"
  s.author             = "Apple"

  s.code_version             = "5.0"
  s.ios.deployment_target     = "9.0"
  s.osx.deployment_target     = "10.10"

  s.source = { git: "https://github.com/apple/swift-builra.git",
               tag: s.version }

  s.default_subspecs = ['Swift']
  s.pod_target_xcconfig = { 
    'OTHER_CFLAGS' => '-I${PODS_TARGET_SRCROOT}/include', 
    'GCC_C_LANGUAGE_STANDARD' => 'c11',
    'CLANG_CXX_LANGUAGE_STANDARD' => 'c++14',
    'CLANG_CXX_LIBRARY' => 'libc++',
  }

  s.subspec 'Swift' do |sp|
    sp.source_files = 'products/builraSwift/**/*.code'
    sp.dependency 'builra/Library'
  end

  s.subspec 'Library' do |sp|
    sp.source_files = 'products/libbuilra/**/*.cpp', 'products/libbuilra/include/builra/*.h'

    # the first is an 'umbrella header', the rest have to be public because 
    # otherwise modular header warnings abound
    sp.public_header_files = 'products/libbuilra/include/builra/builra.h', 'products/libbuilra/include/builra/*.h'
    sp.preserve_paths = 'products/libbuilra/BuildKey-C-API-Private.h', 'include/builra/BuildSystem/{BuildDescription,BuildKey,BuildValue}.h'
    
    sp.dependency 'builra/Core'
    sp.osx.dependency 'builra/BuildSystem' 
  end

  s.subspec 'Core' do |sp|
    sp.source_files = 'lib/Core/**/*.cpp'
    # internal header files, used this way to prevent header clash between subspecs
    sp.preserve_paths = 'include/builra/Core', 'lib/Core/**/*.h'

    sp.libraries = 'sqlite3'
    sp.dependency 'builra/Basic'
  end

  s.subspec 'Basic' do |sp|
    sp.osx.source_files = 'lib/Basic/**/*.cpp'
    sp.ios.source_files = 'lib/Basic/**/{PlatformUtility,Tracing,Version}.cpp'

    # internal header files, used this way to prevent header clash between subspecs
    sp.preserve_paths = 'include/builra/Basic', 'lib/Basic/**/*.h'
    sp.exclude_files = 'include/builra/Basic/LeanWindows.h'

    sp.dependency 'builra/llvmSupport'
  end

  s.subspec 'BuildSystem' do |sp|
    sp.source_files = 'lib/BuildSystem/**/*.cpp'
    # internal header files, used this way to prevent header clash between subspecs
    sp.preserve_paths = 'include/builra/BuildSystem', 'lib/BuildSystem/**/*.h'
    sp.compiler_flags = '-I${PODS_TARGET_SRCROOT}/include'

    sp.dependency 'builra/Core'
  end

  s.subspec 'llvmSupport' do |sp|
    sp.source_files = 'lib/toolchain/{Support,Demangle}/**/*.cpp'
    sp.ios.exclude_files = [
      'lib/toolchain/Support/CommandLine.cpp',
      'lib/toolchain/Support/YAMLParser.cpp',
      'lib/toolchain/Support/SourceMgr.cpp',
      'lib/toolchain/Support/Atomic.cpp',
    ]
    # internal header files, used this way to prevent header clash between subspecs
    sp.preserve_paths = 'include/toolchain', 'include/toolchain-c', 'lib/toolchain/Support/Unix', 'lib/toolchain/Demangle/**/*.h'

    s.osx.libraries = 'ncurses'
  end
end
