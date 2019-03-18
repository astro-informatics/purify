<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile>
  <compound kind="file">
    <name>degridding.cc</name>
    <path>/mydata/cpp/benchmarks/</path>
    <filename>http://astro-informatics.github.io/purify/degridding_8cc</filename>
    <includes id="benchmarks_2utilities_8h" name="utilities.h" local="yes" imported="no">benchmarks/utilities.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <class kind="class">GridOperatorFixture</class>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_DEFINE_F</name>
      <anchorfile>degridding_8cc.html</anchorfile>
      <anchor>a56783e15922302f6e375dcce1cc342e8</anchor>
      <arglist>(GridOperatorFixture, Apply)(benchmark</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>RangeMultiplier</name>
      <anchorfile>degridding_8cc.html</anchorfile>
      <anchor>a07b531175b727759d4409e8726849266</anchor>
      <arglist>(2) -&gt; Range(100000, 100000&lt;&lt; 5) -&gt;UseManualTime() -&gt;Repetitions(10) -&gt;ReportAggregatesOnly(true) -&gt;Unit(benchmark::kMillisecond)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_MAIN</name>
      <anchorfile>degridding_8cc.html</anchorfile>
      <anchor>a5851750faa9cfec10f7cad1f3b89697e</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>fft.cc</name>
    <path>/mydata/cpp/benchmarks/</path>
    <filename>http://astro-informatics.github.io/purify/fft_8cc</filename>
    <includes id="benchmarks_2utilities_8h" name="utilities.h" local="yes" imported="no">benchmarks/utilities.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <class kind="class">FFTOperatorFixture</class>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_DEFINE_F</name>
      <anchorfile>fft_8cc.html</anchorfile>
      <anchor>a9e8e51f62968112b10bed4285c2500db</anchor>
      <arglist>(FFTOperatorFixture, Apply)(benchmark</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>RangeMultiplier</name>
      <anchorfile>fft_8cc.html</anchorfile>
      <anchor>a49cca875081b0ff825a32569b3a715ca</anchor>
      <arglist>(2) -&gt; Range(128, 128&lt;&lt; 6) -&gt;UseManualTime() -&gt;Repetitions(10) -&gt;ReportAggregatesOnly(true) -&gt;Unit(benchmark::kMillisecond)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_MAIN</name>
      <anchorfile>fft_8cc.html</anchorfile>
      <anchor>a5851750faa9cfec10f7cad1f3b89697e</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>main.cc</name>
    <path>/mydata/cpp/benchmarks/</path>
    <filename>http://astro-informatics.github.io/purify/benchmarks_2main_8cc</filename>
    <class kind="class">NullReporter</class>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>benchmarks_2main_8cc.html</anchorfile>
      <anchor>a790aa8b99fa3d90918361b8936af0b14</anchor>
      <arglist>(int argc, char const **argv)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>main.cc</name>
    <path>/mydata/cpp/</path>
    <filename>http://astro-informatics.github.io/purify/main_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="algorithm__factory_8h" name="algorithm_factory.h" local="yes" imported="no">purify/algorithm_factory.h</includes>
    <includes id="cimg_8h" name="cimg.h" local="yes" imported="no">purify/cimg.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="measurement__operator__factory_8h" name="measurement_operator_factory.h" local="yes" imported="no">purify/measurement_operator_factory.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="read__measurements_8h" name="read_measurements.h" local="yes" imported="no">purify/read_measurements.h</includes>
    <includes id="update__factory_8h" name="update_factory.h" local="yes" imported="no">purify/update_factory.h</includes>
    <includes id="wavelet__operator__factory_8h" name="wavelet_operator_factory.h" local="yes" imported="no">purify/wavelet_operator_factory.h</includes>
    <includes id="wide__field__utilities_8h" name="wide_field_utilities.h" local="yes" imported="no">purify/wide_field_utilities.h</includes>
    <includes id="yaml-parser_8h" name="yaml-parser.h" local="yes" imported="no">purify/yaml-parser.h</includes>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>main_8cc.html</anchorfile>
      <anchor>a217dbf8b442f20279ea00b898af96f52</anchor>
      <arglist>(int argc, const char **argv)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>measurement_operator.cc</name>
    <path>/mydata/cpp/benchmarks/</path>
    <filename>http://astro-informatics.github.io/purify/benchmarks_2measurement__operator_8cc</filename>
    <includes id="benchmarks_2utilities_8h" name="utilities.h" local="yes" imported="no">benchmarks/utilities.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <class kind="class">DegridOperatorFixture</class>
    <class kind="class">DegridOperatorDirectFixture</class>
    <class kind="class">DegridOperatorAdjointFixture</class>
    <member kind="function">
      <type>void</type>
      <name>degrid_operator_ctor</name>
      <anchorfile>benchmarks_2measurement__operator_8cc.html</anchorfile>
      <anchor>ab67948162ac703057f32eea708884f66</anchor>
      <arglist>(benchmark::State &amp;state)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Args</name>
      <anchorfile>benchmarks_2measurement__operator_8cc.html</anchorfile>
      <anchor>aa781ac5aa22a08500b9cd6f434cc3640</anchor>
      <arglist>({1024, 1000000, 4}) -&gt; Args(</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>UseManualTime</name>
      <anchorfile>benchmarks_2measurement__operator_8cc.html</anchorfile>
      <anchor>a481329d3f01b443e2827b5af221eb315</anchor>
      <arglist>() -&gt; Repetitions(10) -&gt;Unit(benchmark::kMillisecond)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_DEFINE_F</name>
      <anchorfile>benchmarks_2measurement__operator_8cc.html</anchorfile>
      <anchor>ad2527d1b274d231aaa150dc0323c1e8c</anchor>
      <arglist>(DegridOperatorDirectFixture, Apply)(benchmark</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_DEFINE_F</name>
      <anchorfile>benchmarks_2measurement__operator_8cc.html</anchorfile>
      <anchor>a1338172a3fc4ba7642ffc8d5611858fc</anchor>
      <arglist>(DegridOperatorAdjointFixture, Apply)(benchmark</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_MAIN</name>
      <anchorfile>benchmarks_2measurement__operator_8cc.html</anchorfile>
      <anchor>a5851750faa9cfec10f7cad1f3b89697e</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>measurement_operator.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/tests_2measurement__operator_8cc</filename>
    <includes id="kernels_8h" name="kernels.h" local="yes" imported="no">purify/kernels.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <includes id="wproj__operators_8h" name="wproj_operators.h" local="yes" imported="no">purify/wproj_operators.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2measurement__operator_8cc.html</anchorfile>
      <anchor>aa8d6dc05b8e4d6bf2289201278d3e859</anchor>
      <arglist>(&quot;regression_degrid&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2measurement__operator_8cc.html</anchorfile>
      <anchor>a79bae0670635c162df98f14160b9e02a</anchor>
      <arglist>(&quot;wprojection&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>measurement_operator_af.cc</name>
    <path>/mydata/cpp/benchmarks/</path>
    <filename>http://astro-informatics.github.io/purify/measurement__operator__af_8cc</filename>
    <includes id="benchmarks_2utilities_8h" name="utilities.h" local="yes" imported="no">benchmarks/utilities.h</includes>
    <includes id="operators__gpu_8h" name="operators_gpu.h" local="yes" imported="no">purify/operators_gpu.h</includes>
    <class kind="class">DegridOperatorFixture</class>
    <class kind="class">DegridOperatorDirectFixture</class>
    <class kind="class">DegridOperatorAdjointFixture</class>
    <member kind="function">
      <type>void</type>
      <name>degrid_operator_ctor</name>
      <anchorfile>measurement__operator__af_8cc.html</anchorfile>
      <anchor>ab67948162ac703057f32eea708884f66</anchor>
      <arglist>(benchmark::State &amp;state)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_DEFINE_F</name>
      <anchorfile>measurement__operator__af_8cc.html</anchorfile>
      <anchor>ad2527d1b274d231aaa150dc0323c1e8c</anchor>
      <arglist>(DegridOperatorDirectFixture, Apply)(benchmark</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_DEFINE_F</name>
      <anchorfile>measurement__operator__af_8cc.html</anchorfile>
      <anchor>a1338172a3fc4ba7642ffc8d5611858fc</anchor>
      <arglist>(DegridOperatorAdjointFixture, Apply)(benchmark</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Args</name>
      <anchorfile>measurement__operator__af_8cc.html</anchorfile>
      <anchor>a046865ad202201af44baad38d0b8bcfb</anchor>
      <arglist>({256, 500000, 4}) -&gt; Args(</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Args</name>
      <anchorfile>measurement__operator__af_8cc.html</anchorfile>
      <anchor>a7eda3ab7775b3ce6151a2bda36464fd6</anchor>
      <arglist>({1024, 500000, 4}) -&gt; Args(</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>UseManualTime</name>
      <anchorfile>measurement__operator__af_8cc.html</anchorfile>
      <anchor>a4d7266d864504b8c40b9064db789f8d1</anchor>
      <arglist>() -&gt; Repetitions(10) -&gt;ReportAggregatesOnly(true) -&gt;Unit(benchmark::kMillisecond)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_MAIN</name>
      <anchorfile>measurement__operator__af_8cc.html</anchorfile>
      <anchor>a5851750faa9cfec10f7cad1f3b89697e</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>measurement_operator_mpi.cc</name>
    <path>/mydata/cpp/benchmarks/</path>
    <filename>http://astro-informatics.github.io/purify/measurement__operator__mpi_8cc</filename>
    <includes id="benchmarks_2utilities_8h" name="utilities.h" local="yes" imported="no">benchmarks/utilities.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <class kind="class">DegridOperatorCtorFixturePar</class>
    <class kind="class">DegridOperatorFixturePar</class>
    <class kind="class">DegridOperatorDirectFixturePar</class>
    <class kind="class">DegridOperatorAdjointFixturePar</class>
    <class kind="class">DegridOperatorDirectFixtureDistr</class>
    <class kind="class">DegridOperatorDirectFixtureMPI</class>
    <class kind="class">DegridOperatorAdjointFixtureDistr</class>
    <class kind="class">DegridOperatorAdjointFixtureMPI</class>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_DEFINE_F</name>
      <anchorfile>measurement__operator__mpi_8cc.html</anchorfile>
      <anchor>ad1480f3bda78fed9d8bf382322b89848</anchor>
      <arglist>(DegridOperatorCtorFixturePar, Distr)(benchmark</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_DEFINE_F</name>
      <anchorfile>measurement__operator__mpi_8cc.html</anchorfile>
      <anchor>a775a50347851cf9bfc5349ef699b04d1</anchor>
      <arglist>(DegridOperatorCtorFixturePar, MPI)(benchmark</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_DEFINE_F</name>
      <anchorfile>measurement__operator__mpi_8cc.html</anchorfile>
      <anchor>add1bea0855e8a801e63b2342f15984cf</anchor>
      <arglist>(DegridOperatorDirectFixtureDistr, Apply)(benchmark</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_DEFINE_F</name>
      <anchorfile>measurement__operator__mpi_8cc.html</anchorfile>
      <anchor>ab386477f417b738d65c7fbafa573b5eb</anchor>
      <arglist>(DegridOperatorAdjointFixtureDistr, Apply)(benchmark</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_DEFINE_F</name>
      <anchorfile>measurement__operator__mpi_8cc.html</anchorfile>
      <anchor>a76d62327caee101dce1c87e383303e0f</anchor>
      <arglist>(DegridOperatorDirectFixtureMPI, Apply)(benchmark</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_DEFINE_F</name>
      <anchorfile>measurement__operator__mpi_8cc.html</anchorfile>
      <anchor>a20f3894de307eb480fddcdcdc4bd50e3</anchor>
      <arglist>(DegridOperatorAdjointFixtureMPI, Apply)(benchmark</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Args</name>
      <anchorfile>measurement__operator__mpi_8cc.html</anchorfile>
      <anchor>aa781ac5aa22a08500b9cd6f434cc3640</anchor>
      <arglist>({1024, 1000000, 4}) -&gt; Args(</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>UseManualTime</name>
      <anchorfile>measurement__operator__mpi_8cc.html</anchorfile>
      <anchor>a481329d3f01b443e2827b5af221eb315</anchor>
      <arglist>() -&gt; Repetitions(10) -&gt;Unit(benchmark::kMillisecond)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>padmm.cc</name>
    <path>/mydata/cpp/benchmarks/</path>
    <filename>http://astro-informatics.github.io/purify/padmm_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="benchmarks_2utilities_8h" name="utilities.h" local="yes" imported="no">benchmarks/utilities.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <class kind="class">PadmmFixture</class>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_DEFINE_F</name>
      <anchorfile>padmm_8cc.html</anchorfile>
      <anchor>acc91c46583b8f8fe41a6de94aecb8751</anchor>
      <arglist>(PadmmFixture, Apply)(benchmark</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Args</name>
      <anchorfile>padmm_8cc.html</anchorfile>
      <anchor>aa781ac5aa22a08500b9cd6f434cc3640</anchor>
      <arglist>({1024, 1000000, 4}) -&gt; Args(</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>UseManualTime</name>
      <anchorfile>padmm_8cc.html</anchorfile>
      <anchor>a481329d3f01b443e2827b5af221eb315</anchor>
      <arglist>() -&gt; Repetitions(10) -&gt;Unit(benchmark::kMillisecond)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_MAIN</name>
      <anchorfile>padmm_8cc.html</anchorfile>
      <anchor>a5851750faa9cfec10f7cad1f3b89697e</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>padmm_mpi.cc</name>
    <path>/mydata/cpp/benchmarks/</path>
    <filename>http://astro-informatics.github.io/purify/padmm__mpi_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="benchmarks_2utilities_8h" name="utilities.h" local="yes" imported="no">benchmarks/utilities.h</includes>
    <includes id="convergence__factory_8h" name="convergence_factory.h" local="yes" imported="no">purify/convergence_factory.h</includes>
    <includes id="distribute_8h" name="distribute.h" local="yes" imported="no">purify/distribute.h</includes>
    <includes id="mpi__utilities_8h" name="mpi_utilities.h" local="yes" imported="no">purify/mpi_utilities.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <class kind="class">PadmmFixtureMPI</class>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_DEFINE_F</name>
      <anchorfile>padmm__mpi_8cc.html</anchorfile>
      <anchor>ac4763ec09a987e594fbb79ba85349723</anchor>
      <arglist>(PadmmFixtureMPI, ApplyAlgo1)(benchmark</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_DEFINE_F</name>
      <anchorfile>padmm__mpi_8cc.html</anchorfile>
      <anchor>aecc79116c10a1c94917e3e59fea3a99f</anchor>
      <arglist>(PadmmFixtureMPI, ApplyAlgo3)(benchmark</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Args</name>
      <anchorfile>padmm__mpi_8cc.html</anchorfile>
      <anchor>aa781ac5aa22a08500b9cd6f434cc3640</anchor>
      <arglist>({1024, 1000000, 4}) -&gt; Args(</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>UseManualTime</name>
      <anchorfile>padmm__mpi_8cc.html</anchorfile>
      <anchor>a481329d3f01b443e2827b5af221eb315</anchor>
      <arglist>() -&gt; Repetitions(10) -&gt;Unit(benchmark::kMillisecond)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>utilities.cc</name>
    <path>/mydata/cpp/benchmarks/</path>
    <filename>http://astro-informatics.github.io/purify/benchmarks_2utilities_8cc</filename>
    <includes id="benchmarks_2utilities_8h" name="utilities.h" local="no" imported="no">benchmarks/utilities.h</includes>
    <includes id="distribute_8h" name="distribute.h" local="yes" imported="no">purify/distribute.h</includes>
    <includes id="mpi__utilities_8h" name="mpi_utilities.h" local="yes" imported="no">purify/mpi_utilities.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <namespace>b_utilities</namespace>
    <member kind="function">
      <type>void</type>
      <name>Arguments</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>ab66b84f597f29c434e1666cf4c6f2668</anchor>
      <arglist>(benchmark::internal::Benchmark *b)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>duration</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>ac2c5b1baa8b1154965c0acbe63be33e1</anchor>
      <arglist>(std::chrono::high_resolution_clock::time_point start, std::chrono::high_resolution_clock::time_point end)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>updateImage</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>a8eea3bcb712683446eb05195bda70807</anchor>
      <arglist>(t_uint newSize, Image&lt; t_complex &gt; &amp;image, t_uint &amp;sizex, t_uint &amp;sizey)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>updateEmptyImage</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>aaf7e8fb274c41db4539e54be235ea03a</anchor>
      <arglist>(t_uint newSize, Vector&lt; t_complex &gt; &amp;image, t_uint &amp;sizex, t_uint &amp;sizey)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>updateMeasurements</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>a3298d69bf6039b98f16690284d9689c8</anchor>
      <arglist>(t_uint newSize, utilities::vis_params &amp;data)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>updateMeasurements</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>ab969056ca10702dbc6438b85f9fd583f</anchor>
      <arglist>(t_uint newSize, utilities::vis_params &amp;data, t_real &amp;epsilon, bool newImage, Image&lt; t_complex &gt; &amp;image)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; utilities::vis_params, t_real &gt;</type>
      <name>dirty_measurements</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>a92ba576f33170ef3a76ab53c758a2bed</anchor>
      <arglist>(Image&lt; t_complex &gt; const &amp;ground_truth_image, t_uint number_of_vis, t_real snr, const t_real &amp;cellsize)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>random_measurements</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>a7bf0bd5fea46254ea5c4e81af5c42d1c</anchor>
      <arglist>(t_int size)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>utilities.cc</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/purify_2utilities_8cc</filename>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::utilities</namespace>
    <member kind="function">
      <type>Matrix&lt; t_real &gt;</type>
      <name>generate_antennas</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a14f77d27cb3daf69f4d53a1bc551d6ec</anchor>
      <arglist>(const t_uint N)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>antenna_to_coverage</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a0a278d13ff1ec7afe890960a836fe8b8</anchor>
      <arglist>(const t_uint N)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>antenna_to_coverage</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>aa38e259a0d826280ef79d31dfb897a60</anchor>
      <arglist>(const Matrix&lt; t_real &gt; &amp;B)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>random_sample_density</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ae8446d8749e342037d66da0ad3ca63ad</anchor>
      <arglist>(const t_int vis_num, const t_real mean, const t_real standard_deviation, const t_real rms_w=0)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_visibility</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ad4492cfc4a58430ffa09a7c002db7988</anchor>
      <arglist>(const std::vector&lt; std::string &gt; &amp;names, const bool w_term=false)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_visibility</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>abbad783bede3e70d3d62520cac7cdc76</anchor>
      <arglist>(const std::string &amp;vis_name2, const utilities::vis_params &amp;u1)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>streamtoreal</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a796bee1239c7873850c229f57d637189</anchor>
      <arglist>(std::ifstream &amp;stream)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_visibility</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>aae90971eb2fdab205c39d54868ce66e5</anchor>
      <arglist>(const std::string &amp;vis_name, const bool w_term=false)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write_visibility</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a083e82d5b1fdc9cae30da55252492c0f</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis, const std::string &amp;file_name, const bool w_term=false)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>set_cell_size</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a9fe18dbda5ae6286e280b563977c0fae</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis, const t_real &amp;max_u, const t_real &amp;max_v, const t_real &amp;input_cell_size_u, const t_real &amp;input_cell_size_v)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>set_cell_size</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a97777e616ef68893b4b62677cf2286fe</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis, const t_real &amp;cell_size_u=0, const t_real &amp;cell_size_v=0)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>uv_scale</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a5bc6a589888c5f3cc468acbb0c3bee37</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis, const t_int &amp;ftsizeu, const t_int &amp;ftsizev)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>conjugate_w</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a8f267ff244270871a0d36b52ff9b3eb6</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis)</arglist>
    </member>
    <member kind="function">
      <type>t_int</type>
      <name>sub2ind</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a7d6e0055d484aac9b7728a6614a80a07</anchor>
      <arglist>(const t_int &amp;row, const t_int &amp;col, const t_int &amp;rows, const t_int &amp;cols)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; t_int, t_int &gt;</type>
      <name>ind2sub</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>afb213bdf3ecdb4d9557e810349443234</anchor>
      <arglist>(const t_int &amp;sub, const t_int &amp;cols, const t_int &amp;rows)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>mod</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ae7983367cb937c38c31b16bd1e4619fa</anchor>
      <arglist>(const t_real &amp;x, const t_real &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Image&lt; t_complex &gt;</type>
      <name>convolution_operator</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>af717a83833a9f2025be0d26ef8f08788</anchor>
      <arglist>(const Image&lt; t_complex &gt; &amp;a, const Image&lt; t_complex &gt; &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>whiten_vis</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ae894867ada4d1a22fb98025e8260cc71</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>calculate_l2_radius</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>af90182ef1213164564e72e51f8a83ac7</anchor>
      <arglist>(const t_uint y_size, const t_real &amp;sigma=0, const t_real &amp;n_sigma=2., const std::string distirbution=&quot;chi&quot;)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>SNR_to_standard_deviation</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a4b530b0918705248f2d2dd4de0bb29f6</anchor>
      <arglist>(const Vector&lt; t_complex &gt; &amp;y0, const t_real &amp;SNR=30.)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_complex &gt;</type>
      <name>add_noise</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a7788412bbecc48c81b8e5777943b5666</anchor>
      <arglist>(const Vector&lt; t_complex &gt; &amp;y, const t_complex &amp;mean, const t_real &amp;standard_deviation)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>file_exists</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>aa251252a271965c9568d3f3bd8e37259</anchor>
      <arglist>(const std::string &amp;name)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_real &gt;</type>
      <name>fit_fwhm</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a9a837da30f295946510cbeccf9bf4abf</anchor>
      <arglist>(const Image&lt; t_real &gt; &amp;psf, const t_int &amp;size=3)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>median</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a1e331f75754f76388cfbf6e586f8d276</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;input)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>dynamic_range</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ad55d48f38b4f24dfc7d703682fb3e26d</anchor>
      <arglist>(const Image&lt; t_complex &gt; &amp;model, const Image&lt; t_complex &gt; &amp;residuals, const t_real &amp;operator_norm=1)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; t_complex &gt;</type>
      <name>init_weights</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a0bb5cc817f651a8bf2d9eebdfbf91caf</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;u, const Vector&lt; t_real &gt; &amp;v, const Vector&lt; t_complex &gt; &amp;weights, const t_real &amp;oversample_factor, const std::string &amp;weighting_type, const t_real &amp;R, const t_int &amp;ftsizeu, const t_int &amp;ftsizev)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; t_int, t_real &gt;</type>
      <name>checkpoint_log</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a90ba5f2c558a7c02319096b2ad7f48b1</anchor>
      <arglist>(const std::string &amp;diagnostic)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>re_sample_ft_grid</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a99180a66ed0f610008564648b73057b5</anchor>
      <arglist>(const Matrix&lt; t_complex &gt; &amp;input, const t_real &amp;re_sample_factor)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>re_sample_image</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a9f6f39cdfdf8c9a8136f69ec144d2e47</anchor>
      <arglist>(const Matrix&lt; t_complex &gt; &amp;input, const t_real &amp;re_sample_ratio)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>utilities.h</name>
    <path>/mydata/cpp/benchmarks/</path>
    <filename>http://astro-informatics.github.io/purify/benchmarks_2utilities_8h</filename>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <namespace>b_utilities</namespace>
    <member kind="function">
      <type>void</type>
      <name>Arguments</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>ab66b84f597f29c434e1666cf4c6f2668</anchor>
      <arglist>(benchmark::internal::Benchmark *b)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>duration</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>ac2c5b1baa8b1154965c0acbe63be33e1</anchor>
      <arglist>(std::chrono::high_resolution_clock::time_point start, std::chrono::high_resolution_clock::time_point end)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>updateImage</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>a8eea3bcb712683446eb05195bda70807</anchor>
      <arglist>(t_uint newSize, Image&lt; t_complex &gt; &amp;image, t_uint &amp;sizex, t_uint &amp;sizey)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>updateEmptyImage</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>aaf7e8fb274c41db4539e54be235ea03a</anchor>
      <arglist>(t_uint newSize, Vector&lt; t_complex &gt; &amp;image, t_uint &amp;sizex, t_uint &amp;sizey)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>updateMeasurements</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>a3298d69bf6039b98f16690284d9689c8</anchor>
      <arglist>(t_uint newSize, utilities::vis_params &amp;data)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>updateMeasurements</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>ab969056ca10702dbc6438b85f9fd583f</anchor>
      <arglist>(t_uint newSize, utilities::vis_params &amp;data, t_real &amp;epsilon, bool newImage, Image&lt; t_complex &gt; &amp;image)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; utilities::vis_params, t_real &gt;</type>
      <name>dirty_measurements</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>a92ba576f33170ef3a76ab53c758a2bed</anchor>
      <arglist>(Image&lt; t_complex &gt; const &amp;ground_truth_image, t_uint number_of_vis, t_real snr, const t_real &amp;cellsize)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>random_measurements</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>a7bf0bd5fea46254ea5c4e81af5c42d1c</anchor>
      <arglist>(t_int size)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>utilities.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/purify_2utilities_8h</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <class kind="struct">purify::utilities::vis_params</class>
    <namespace>purify</namespace>
    <namespace>purify::utilities</namespace>
    <member kind="enumeration">
      <type></type>
      <name>vis_source</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>af608b7e6a570ae7289e797c577850a64</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html" anchor="af608b7e6a570ae7289e797c577850a64ac37be74d84f98bd288693a6bbee7a334">measurements</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html" anchor="af608b7e6a570ae7289e797c577850a64a9e083ec666c9f3db044bb7c381640227">simulation</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>vis_units</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>aa2af2ee7e4f1bb93a42235b9d973983d</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html" anchor="aa2af2ee7e4f1bb93a42235b9d973983da945f3fc449518a73b9f5f32868db466c">lambda</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html" anchor="aa2af2ee7e4f1bb93a42235b9d973983da017969ba49117264eb077e9f66daacfd">radians</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html" anchor="aa2af2ee7e4f1bb93a42235b9d973983dad399848208da8b80a306af0fd62bb03f">pixels</enumvalue>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>random_sample_density</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ae8446d8749e342037d66da0ad3ca63ad</anchor>
      <arglist>(const t_int vis_num, const t_real mean, const t_real standard_deviation, const t_real rms_w=0)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_real &gt;</type>
      <name>generate_antennas</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a14f77d27cb3daf69f4d53a1bc551d6ec</anchor>
      <arglist>(const t_uint N)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>antenna_to_coverage</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a0a278d13ff1ec7afe890960a836fe8b8</anchor>
      <arglist>(const t_uint N)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>antenna_to_coverage</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>aa38e259a0d826280ef79d31dfb897a60</anchor>
      <arglist>(const Matrix&lt; t_real &gt; &amp;B)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>streamtoreal</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a796bee1239c7873850c229f57d637189</anchor>
      <arglist>(std::ifstream &amp;stream)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_visibility</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>aae90971eb2fdab205c39d54868ce66e5</anchor>
      <arglist>(const std::string &amp;vis_name, const bool w_term=false)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_visibility</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ad4492cfc4a58430ffa09a7c002db7988</anchor>
      <arglist>(const std::vector&lt; std::string &gt; &amp;names, const bool w_term=false)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_visibility</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>abbad783bede3e70d3d62520cac7cdc76</anchor>
      <arglist>(const std::string &amp;vis_name2, const utilities::vis_params &amp;u1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write_visibility</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a083e82d5b1fdc9cae30da55252492c0f</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis, const std::string &amp;file_name, const bool w_term=false)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>set_cell_size</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a97777e616ef68893b4b62677cf2286fe</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis, const t_real &amp;cell_size_u=0, const t_real &amp;cell_size_v=0)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>set_cell_size</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a9fe18dbda5ae6286e280b563977c0fae</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis, const t_real &amp;max_u, const t_real &amp;max_v, const t_real &amp;input_cell_size_u, const t_real &amp;input_cell_size_v)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>uv_scale</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a5bc6a589888c5f3cc468acbb0c3bee37</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis, const t_int &amp;ftsizeu, const t_int &amp;ftsizev)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>conjugate_w</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a8f267ff244270871a0d36b52ff9b3eb6</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis)</arglist>
    </member>
    <member kind="function">
      <type>t_int</type>
      <name>sub2ind</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a7d6e0055d484aac9b7728a6614a80a07</anchor>
      <arglist>(const t_int &amp;row, const t_int &amp;col, const t_int &amp;rows, const t_int &amp;cols)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; t_int, t_int &gt;</type>
      <name>ind2sub</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>afb213bdf3ecdb4d9557e810349443234</anchor>
      <arglist>(const t_int &amp;sub, const t_int &amp;cols, const t_int &amp;rows)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>mod</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ae7983367cb937c38c31b16bd1e4619fa</anchor>
      <arglist>(const t_real &amp;x, const t_real &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>K::Scalar</type>
      <name>mean</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a616adaec8411e901dfaf0e2728f1a9dc</anchor>
      <arglist>(const K x)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>variance</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a327605e8997b50446f40a76434764e88</anchor>
      <arglist>(const K x)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>standard_deviation</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ae9a47127ad2c0c43f8387e0da2cf804b</anchor>
      <arglist>(const K x)</arglist>
    </member>
    <member kind="function">
      <type>Image&lt; t_complex &gt;</type>
      <name>convolution_operator</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>af717a83833a9f2025be0d26ef8f08788</anchor>
      <arglist>(const Image&lt; t_complex &gt; &amp;a, const Image&lt; t_complex &gt; &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>whiten_vis</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ae894867ada4d1a22fb98025e8260cc71</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>calculate_l2_radius</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>af90182ef1213164564e72e51f8a83ac7</anchor>
      <arglist>(const t_uint y_size, const t_real &amp;sigma=0, const t_real &amp;n_sigma=2., const std::string distirbution=&quot;chi&quot;)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>SNR_to_standard_deviation</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a4b530b0918705248f2d2dd4de0bb29f6</anchor>
      <arglist>(const Vector&lt; t_complex &gt; &amp;y0, const t_real &amp;SNR=30.)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_complex &gt;</type>
      <name>add_noise</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a7788412bbecc48c81b8e5777943b5666</anchor>
      <arglist>(const Vector&lt; t_complex &gt; &amp;y, const t_complex &amp;mean, const t_real &amp;standard_deviation)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>file_exists</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>aa251252a271965c9568d3f3bd8e37259</anchor>
      <arglist>(const std::string &amp;name)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_real &gt;</type>
      <name>fit_fwhm</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a9a837da30f295946510cbeccf9bf4abf</anchor>
      <arglist>(const Image&lt; t_real &gt; &amp;psf, const t_int &amp;size=3)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>median</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a1e331f75754f76388cfbf6e586f8d276</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;input)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>dynamic_range</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ad55d48f38b4f24dfc7d703682fb3e26d</anchor>
      <arglist>(const Image&lt; t_complex &gt; &amp;model, const Image&lt; t_complex &gt; &amp;residuals, const t_real &amp;operator_norm=1)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; t_complex &gt;</type>
      <name>init_weights</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a0bb5cc817f651a8bf2d9eebdfbf91caf</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;u, const Vector&lt; t_real &gt; &amp;v, const Vector&lt; t_complex &gt; &amp;weights, const t_real &amp;oversample_factor, const std::string &amp;weighting_type, const t_real &amp;R, const t_int &amp;ftsizeu, const t_int &amp;ftsizev)</arglist>
    </member>
    <member kind="function">
      <type>std::enable_if&lt; std::is_same&lt; typename T0::Scalar, typename T1::Scalar &gt;::value and T0::IsRowMajor, Vector&lt; typename T0::Scalar &gt; &gt;::type</type>
      <name>sparse_multiply_matrix</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a0ff3f9b89549f7e5a46cf56524e4e7cc</anchor>
      <arglist>(const Eigen::SparseMatrixBase&lt; T0 &gt; &amp;M, const Eigen::MatrixBase&lt; T1 &gt; &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; t_int, t_real &gt;</type>
      <name>checkpoint_log</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a90ba5f2c558a7c02319096b2ad7f48b1</anchor>
      <arglist>(const std::string &amp;diagnostic)</arglist>
    </member>
    <member kind="function">
      <type>Image&lt; t_complex &gt;</type>
      <name>parallel_multiply_image</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>afff87a1bc7664b5277e71cbf7f7f217f</anchor>
      <arglist>(const K &amp;A, const L &amp;B)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>re_sample_ft_grid</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a99180a66ed0f610008564648b73057b5</anchor>
      <arglist>(const Matrix&lt; t_complex &gt; &amp;input, const t_real &amp;re_sample_factor)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>re_sample_image</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a9f6f39cdfdf8c9a8136f69ec144d2e47</anchor>
      <arglist>(const Matrix&lt; t_complex &gt; &amp;input, const t_real &amp;re_sample_ratio)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>wavelet_operator.cc</name>
    <path>/mydata/cpp/benchmarks/</path>
    <filename>http://astro-informatics.github.io/purify/wavelet__operator_8cc</filename>
    <includes id="benchmarks_2utilities_8h" name="utilities.h" local="yes" imported="no">benchmarks/utilities.h</includes>
    <includes id="wavelet__operator__factory_8h" name="wavelet_operator_factory.h" local="yes" imported="no">purify/wavelet_operator_factory.h</includes>
    <class kind="class">WaveletOperatorFixture</class>
    <class kind="class">WaveletOperatorAdjointFixture</class>
    <member kind="function">
      <type>void</type>
      <name>wavelet_operator_constructor</name>
      <anchorfile>wavelet__operator_8cc.html</anchorfile>
      <anchor>a0d678d5151f5123a595b8176ce204434</anchor>
      <arglist>(benchmark::State &amp;state)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>RangeMultiplier</name>
      <anchorfile>wavelet__operator_8cc.html</anchorfile>
      <anchor>a5fcd101275d7274f057eac9599d93c9e</anchor>
      <arglist>(2) -&gt; Range(1024, 1024&lt;&lt; 10) -&gt;UseManualTime() -&gt;Repetitions(10) -&gt;ReportAggregatesOnly(true) -&gt;Unit(benchmark::kMillisecond)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_DEFINE_F</name>
      <anchorfile>wavelet__operator_8cc.html</anchorfile>
      <anchor>a2eaa2b3e643099bf2e2b0ec0b6c01360</anchor>
      <arglist>(WaveletOperatorFixture, Apply)(benchmark</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_DEFINE_F</name>
      <anchorfile>wavelet__operator_8cc.html</anchorfile>
      <anchor>a3e0943519b68d4513885dca6bf4986e8</anchor>
      <arglist>(WaveletOperatorAdjointFixture, Apply)(benchmark</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_MAIN</name>
      <anchorfile>wavelet__operator_8cc.html</anchorfile>
      <anchor>a5851750faa9cfec10f7cad1f3b89697e</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>wavelet_operator_mpi.cc</name>
    <path>/mydata/cpp/benchmarks/</path>
    <filename>http://astro-informatics.github.io/purify/wavelet__operator__mpi_8cc</filename>
    <includes id="benchmarks_2utilities_8h" name="utilities.h" local="yes" imported="no">benchmarks/utilities.h</includes>
    <includes id="wavelet__operator__factory_8h" name="wavelet_operator_factory.h" local="yes" imported="no">purify/wavelet_operator_factory.h</includes>
    <class kind="class">WaveletOperatorMPIFixture</class>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_DEFINE_F</name>
      <anchorfile>wavelet__operator__mpi_8cc.html</anchorfile>
      <anchor>a3594f7598217584d2b64fb3207245058</anchor>
      <arglist>(WaveletOperatorMPIFixture, Forward)(benchmark</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BENCHMARK_DEFINE_F</name>
      <anchorfile>wavelet__operator__mpi_8cc.html</anchorfile>
      <anchor>ac0ce4fff818fca0e632d3a83882845de</anchor>
      <arglist>(WaveletOperatorMPIFixture, Adjoint)(benchmark</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>RangeMultiplier</name>
      <anchorfile>wavelet__operator__mpi_8cc.html</anchorfile>
      <anchor>a332bd5e1fb40bf0ed25a99c3d54f5644</anchor>
      <arglist>(2) -&gt; Range(128, 128&lt;&lt; 3) -&gt;UseManualTime() -&gt;Repetitions(5) -&gt;ReportAggregatesOnly(true) -&gt;Unit(benchmark::kMillisecond)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>PURIFY_CONTRIBUTORS.md</name>
    <path>/mydata/cpp/docs/</path>
    <filename>http://astro-informatics.github.io/purify/PURIFY__CONTRIBUTORS_8md</filename>
  </compound>
  <compound kind="file">
    <name>README.md</name>
    <path>/mydata/cpp/docs/</path>
    <filename>http://astro-informatics.github.io/purify/README_8md</filename>
  </compound>
  <compound kind="file">
    <name>casa.cc</name>
    <path>/mydata/cpp/example/</path>
    <filename>http://astro-informatics.github.io/purify/casa_8cc</filename>
    <includes id="casacore_8h" name="casacore.h" local="yes" imported="no">purify/casacore.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>CHECK</name>
      <anchorfile>casa_8cc.html</anchorfile>
      <anchor>a2f04a5b42b147fe2ac9c62e73fa66364</anchor>
      <arglist>(CONDITION, ERROR)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>CHECK_THROWS</name>
      <anchorfile>casa_8cc.html</anchorfile>
      <anchor>a930f53190e06c98005bf90947b0aff89</anchor>
      <arglist>(STATEMENT, ERROR)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>casa_8cc.html</anchorfile>
      <anchor>a2c3f6775325c30275d11c6abee2db6a0</anchor>
      <arglist>(int, char **)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>compare_wprojection.cc</name>
    <path>/mydata/cpp/example/</path>
    <filename>http://astro-informatics.github.io/purify/compare__wprojection_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <includes id="wide__field__utilities_8h" name="wide_field_utilities.h" local="yes" imported="no">purify/wide_field_utilities.h</includes>
    <includes id="wproj__operators_8h" name="wproj_operators.h" local="yes" imported="no">purify/wproj_operators.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>ARGS_MACRO</name>
      <anchorfile>compare__wprojection_8cc.html</anchorfile>
      <anchor>a5ef35d83033741b88fd4635a3ffe60de</anchor>
      <arglist>(NAME, ARGN, VALUE, TYPE)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>compare__wprojection_8cc.html</anchorfile>
      <anchor>aea57d6e3bfb4b5e83027443146631164</anchor>
      <arglist>(int nargs, char const **args)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>generate_vis_data.cc</name>
    <path>/mydata/cpp/example/</path>
    <filename>http://astro-informatics.github.io/purify/generate__vis__data_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>generate__vis__data_8cc.html</anchorfile>
      <anchor>aea57d6e3bfb4b5e83027443146631164</anchor>
      <arglist>(int nargs, char const **args)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>gridding.cc</name>
    <path>/mydata/cpp/example/</path>
    <filename>http://astro-informatics.github.io/purify/gridding_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>gridding_8cc.html</anchorfile>
      <anchor>aea57d6e3bfb4b5e83027443146631164</anchor>
      <arglist>(int nargs, char const **args)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>histogram_equalisation.cc</name>
    <path>/mydata/cpp/example/</path>
    <filename>http://astro-informatics.github.io/purify/histogram__equalisation_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="cimg_8h" name="cimg.h" local="yes" imported="no">purify/cimg.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>histogram__equalisation_8cc.html</anchorfile>
      <anchor>aea57d6e3bfb4b5e83027443146631164</anchor>
      <arglist>(int nargs, char const **args)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>image_wproj_chirp.cc</name>
    <path>/mydata/cpp/example/</path>
    <filename>http://astro-informatics.github.io/purify/image__wproj__chirp_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <includes id="wide__field__utilities_8h" name="wide_field_utilities.h" local="yes" imported="no">purify/wide_field_utilities.h</includes>
    <includes id="wproj__operators_8h" name="wproj_operators.h" local="yes" imported="no">purify/wproj_operators.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>ARGS_MACRO</name>
      <anchorfile>image__wproj__chirp_8cc.html</anchorfile>
      <anchor>a5ef35d83033741b88fd4635a3ffe60de</anchor>
      <arglist>(NAME, ARGN, VALUE, TYPE)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>image__wproj__chirp_8cc.html</anchorfile>
      <anchor>aea57d6e3bfb4b5e83027443146631164</anchor>
      <arglist>(int nargs, char const **args)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>padmm_mpi_random_coverage.cc</name>
    <path>/mydata/cpp/example/</path>
    <filename>http://astro-informatics.github.io/purify/padmm__mpi__random__coverage_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="distribute_8h" name="distribute.h" local="yes" imported="no">purify/distribute.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="mpi__utilities_8h" name="mpi_utilities.h" local="yes" imported="no">purify/mpi_utilities.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="read__measurements_8h" name="read_measurements.h" local="yes" imported="no">purify/read_measurements.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_PADMM_ALGORITHM</name>
      <anchorfile>padmm__mpi__random__coverage_8cc.html</anchorfile>
      <anchor>a361e852a4c304de1f7ee66e0b6a5f59a</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; utilities::vis_params, t_real &gt;</type>
      <name>dirty_visibilities</name>
      <anchorfile>padmm__mpi__random__coverage_8cc.html</anchorfile>
      <anchor>a61f82452b4250be14d08fdabbde354a6</anchor>
      <arglist>(Image&lt; t_complex &gt; const &amp;ground_truth_image, t_uint number_of_vis, t_real snr, const std::tuple&lt; bool, t_real &gt; &amp;w_term)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; utilities::vis_params, t_real &gt;</type>
      <name>dirty_visibilities</name>
      <anchorfile>padmm__mpi__random__coverage_8cc.html</anchorfile>
      <anchor>a919a91cfc9fbefd7ebd52583c318036c</anchor>
      <arglist>(Image&lt; t_complex &gt; const &amp;ground_truth_image, t_uint number_of_vis, t_real snr, const std::tuple&lt; bool, t_real &gt; &amp;w_term, sopt::mpi::Communicator const &amp;comm)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; sopt::algorithm::ImagingProximalADMM&lt; t_complex &gt; &gt;</type>
      <name>padmm_factory</name>
      <anchorfile>padmm__mpi__random__coverage_8cc.html</anchorfile>
      <anchor>a0ba318e6f18f25ce53234cb6a56dbeb5</anchor>
      <arglist>(std::shared_ptr&lt; sopt::LinearTransform&lt; Vector&lt; t_complex &gt;&gt; const &gt; const &amp;measurements, const sopt::wavelets::SARA &amp;sara, const Image&lt; t_complex &gt; &amp;ground_truth_image, const utilities::vis_params &amp;uv_data, const t_real sigma, const sopt::mpi::Communicator &amp;comm)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>padmm__mpi__random__coverage_8cc.html</anchorfile>
      <anchor>aea57d6e3bfb4b5e83027443146631164</anchor>
      <arglist>(int nargs, char const **args)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>padmm_mpi_real_data.cc</name>
    <path>/mydata/cpp/example/</path>
    <filename>http://astro-informatics.github.io/purify/padmm__mpi__real__data_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="distribute_8h" name="distribute.h" local="yes" imported="no">purify/distribute.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="mpi__utilities_8h" name="mpi_utilities.h" local="yes" imported="no">purify/mpi_utilities.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="read__measurements_8h" name="read_measurements.h" local="yes" imported="no">purify/read_measurements.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <includes id="uvfits_8h" name="uvfits.h" local="yes" imported="no">purify/uvfits.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_PADMM_ALGORITHM</name>
      <anchorfile>padmm__mpi__real__data_8cc.html</anchorfile>
      <anchor>a361e852a4c304de1f7ee66e0b6a5f59a</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>dirty_visibilities</name>
      <anchorfile>padmm__mpi__real__data_8cc.html</anchorfile>
      <anchor>afdd6549b257bd91a43bf98d8654cf0cd</anchor>
      <arglist>(const std::vector&lt; std::string &gt; &amp;names)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>dirty_visibilities</name>
      <anchorfile>padmm__mpi__real__data_8cc.html</anchorfile>
      <anchor>a571c9ca1effca2d32f82a7ffabd6c30e</anchor>
      <arglist>(const std::vector&lt; std::string &gt; &amp;names, sopt::mpi::Communicator const &amp;comm)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; sopt::algorithm::ImagingProximalADMM&lt; t_complex &gt; &gt;</type>
      <name>padmm_factory</name>
      <anchorfile>padmm__mpi__real__data_8cc.html</anchorfile>
      <anchor>a1743376a02975e0273f05cb8b452ffc8</anchor>
      <arglist>(std::shared_ptr&lt; sopt::LinearTransform&lt; Vector&lt; t_complex &gt;&gt; const &gt; const &amp;measurements, t_real const sigma, const sopt::wavelets::SARA &amp;sara, const utilities::vis_params &amp;uv_data, const sopt::mpi::Communicator &amp;comm, const t_uint &amp;imsizex, const t_uint &amp;imsizey)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>padmm__mpi__real__data_8cc.html</anchorfile>
      <anchor>aea57d6e3bfb4b5e83027443146631164</anchor>
      <arglist>(int nargs, char const **args)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>padmm_random_coverage.cc</name>
    <path>/mydata/cpp/example/</path>
    <filename>http://astro-informatics.github.io/purify/padmm__random__coverage_8cc</filename>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="wproj__operators_8h" name="wproj_operators.h" local="yes" imported="no">purify/wproj_operators.h</includes>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="cimg_8h" name="cimg.h" local="yes" imported="no">purify/cimg.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <member kind="function">
      <type>void</type>
      <name>padmm</name>
      <anchorfile>padmm__random__coverage_8cc.html</anchorfile>
      <anchor>a0a5a4b5d505c1171d7dcabe5269fabb1</anchor>
      <arglist>(const std::string &amp;name, const Image&lt; t_complex &gt; &amp;M31, const std::string &amp;kernel, const t_int J, const utilities::vis_params &amp;uv_data, const t_real sigma, const std::tuple&lt; bool, t_real &gt; &amp;w_term)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>padmm__random__coverage_8cc.html</anchorfile>
      <anchor>a2c3f6775325c30275d11c6abee2db6a0</anchor>
      <arglist>(int, char **)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>padmm_real_data.cc</name>
    <path>/mydata/cpp/example/</path>
    <filename>http://astro-informatics.github.io/purify/padmm__real__data_8cc</filename>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="cimg_8h" name="cimg.h" local="yes" imported="no">purify/cimg.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <includes id="uvfits_8h" name="uvfits.h" local="yes" imported="no">purify/uvfits.h</includes>
    <includes id="wproj__utilities_8h" name="wproj_utilities.h" local="yes" imported="no">purify/wproj_utilities.h</includes>
    <member kind="function">
      <type>void</type>
      <name>padmm</name>
      <anchorfile>padmm__real__data_8cc.html</anchorfile>
      <anchor>a4c69bab64427e545ab5b8817dcdac66d</anchor>
      <arglist>(const std::string &amp;name, const t_uint &amp;imsizex, const t_uint &amp;imsizey, const std::string &amp;kernel, const t_int J, const utilities::vis_params &amp;uv_data, const t_real sigma, const std::tuple&lt; bool, t_real &gt; &amp;w_term)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>padmm__real__data_8cc.html</anchorfile>
      <anchor>a2c3f6775325c30275d11c6abee2db6a0</anchor>
      <arglist>(int, char **)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>padmm_reweighted_simulation.cc</name>
    <path>/mydata/cpp/example/</path>
    <filename>http://astro-informatics.github.io/purify/padmm__reweighted__simulation_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>padmm__reweighted__simulation_8cc.html</anchorfile>
      <anchor>aea57d6e3bfb4b5e83027443146631164</anchor>
      <arglist>(int nargs, char const **args)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>padmm_simulation.cc</name>
    <path>/mydata/cpp/example/</path>
    <filename>http://astro-informatics.github.io/purify/padmm__simulation_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>padmm__simulation_8cc.html</anchorfile>
      <anchor>aea57d6e3bfb4b5e83027443146631164</anchor>
      <arglist>(int nargs, char const **args)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>plot_wkernel.cc</name>
    <path>/mydata/cpp/example/</path>
    <filename>http://astro-informatics.github.io/purify/plot__wkernel_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="kernels_8h" name="kernels.h" local="yes" imported="no">purify/kernels.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="wide__field__utilities_8h" name="wide_field_utilities.h" local="yes" imported="no">purify/wide_field_utilities.h</includes>
    <includes id="wkernel__integration_8h" name="wkernel_integration.h" local="yes" imported="no">purify/wkernel_integration.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>ARGS_MACRO</name>
      <anchorfile>plot__wkernel_8cc.html</anchorfile>
      <anchor>a5ef35d83033741b88fd4635a3ffe60de</anchor>
      <arglist>(NAME, ARGN, VALUE, TYPE)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>plot__wkernel_8cc.html</anchorfile>
      <anchor>aea57d6e3bfb4b5e83027443146631164</anchor>
      <arglist>(int nargs, char const **args)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>sara_padmm_random_coverage.cc</name>
    <path>/mydata/cpp/example/</path>
    <filename>http://astro-informatics.github.io/purify/sara__padmm__random__coverage_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>sara__padmm__random__coverage_8cc.html</anchorfile>
      <anchor>a2c3f6775325c30275d11c6abee2db6a0</anchor>
      <arglist>(int, char **)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>sdmm_m31_simulation.cc</name>
    <path>/mydata/cpp/example/</path>
    <filename>http://astro-informatics.github.io/purify/sdmm__m31__simulation_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>sdmm__m31__simulation_8cc.html</anchorfile>
      <anchor>aea57d6e3bfb4b5e83027443146631164</anchor>
      <arglist>(int nargs, char const **args)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>show_image.cc</name>
    <path>/mydata/cpp/example/</path>
    <filename>http://astro-informatics.github.io/purify/show__image_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="cimg_8h" name="cimg.h" local="yes" imported="no">purify/cimg.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>show__image_8cc.html</anchorfile>
      <anchor>aea57d6e3bfb4b5e83027443146631164</anchor>
      <arglist>(int nargs, char const **args)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>wavelet_decomposition.cc</name>
    <path>/mydata/cpp/example/</path>
    <filename>http://astro-informatics.github.io/purify/wavelet__decomposition_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <includes id="wavelet__operator__factory_8h" name="wavelet_operator_factory.h" local="yes" imported="no">purify/wavelet_operator_factory.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>ARGS_MACRO</name>
      <anchorfile>wavelet__decomposition_8cc.html</anchorfile>
      <anchor>a5ef35d83033741b88fd4635a3ffe60de</anchor>
      <arglist>(NAME, ARGN, VALUE, TYPE)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>wavelet__decomposition_8cc.html</anchorfile>
      <anchor>aea57d6e3bfb4b5e83027443146631164</anchor>
      <arglist>(int nargs, char const **args)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>algorithm_factory.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/algorithm__factory_8h</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="convergence__factory_8h" name="convergence_factory.h" local="yes" imported="no">purify/convergence_factory.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::factory</namespace>
    <member kind="enumeration">
      <type></type>
      <name>algorithm</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>a61804e4c4f79fa94c6b864535459d5f8</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a61804e4c4f79fa94c6b864535459d5f8af6ff4586be2e5d12bbd6447df9719d9b">padmm</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a61804e4c4f79fa94c6b864535459d5f8a5de8b4afa495633c7ddf39c69642d187">primal_dual</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a61804e4c4f79fa94c6b864535459d5f8a610b120e176bdf628308e7f0f4ab3b7b">sdmm</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a61804e4c4f79fa94c6b864535459d5f8a9fd3baacb5ca6344ddb0ab9f5b71cb02">forward_backward</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>algo_distribution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>a862f0cad917b1ba718f965fd508f31f4</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a862f0cad917b1ba718f965fd508f31f4a74136b24217cb75599440e6e1d807cf7">serial</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a862f0cad917b1ba718f965fd508f31f4ac0fed08bcb99b1cd719f49ff80e9a912">mpi_serial</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a862f0cad917b1ba718f965fd508f31f4afd592d6cd0b4eadd3f6fa268b995ff76">mpi_distributed</enumvalue>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; Algorithm &gt;</type>
      <name>algorithm_factory</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>aab5d94490e8d79a279fa050f697a13a1</anchor>
      <arglist>(const factory::algorithm algo, ARGS &amp;&amp;... args)</arglist>
    </member>
    <member kind="function">
      <type>std::enable_if&lt; std::is_same&lt; Algorithm, sopt::algorithm::ImagingProximalADMM&lt; t_complex &gt; &gt;::value, std::shared_ptr&lt; Algorithm &gt; &gt;::type</type>
      <name>padmm_factory</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>ad14f66077e408cf7ea6015983bed1383</anchor>
      <arglist>(const algo_distribution dist, std::shared_ptr&lt; sopt::LinearTransform&lt; Vector&lt; typename Algorithm::Scalar &gt;&gt; const &gt; const &amp;measurements, std::shared_ptr&lt; sopt::LinearTransform&lt; Vector&lt; typename Algorithm::Scalar &gt;&gt; const &gt; const &amp;wavelets, const utilities::vis_params &amp;uv_data, const t_real sigma, const t_uint imsizey, const t_uint imsizex, const t_uint sara_size, const t_uint max_iterations=500, const bool real_constraint=true, const bool positive_constraint=true, const bool tight_frame=false, const t_real relative_variation=1e-3, const t_real l1_proximal_tolerance=1e-2, const t_uint maximum_proximal_iterations=50)</arglist>
    </member>
    <member kind="function">
      <type>std::enable_if&lt; std::is_same&lt; Algorithm, sopt::algorithm::ImagingForwardBackward&lt; t_complex &gt; &gt;::value, std::shared_ptr&lt; Algorithm &gt; &gt;::type</type>
      <name>fb_factory</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>aa6f2e06bcd3586e4583b44f1feb07abd</anchor>
      <arglist>(const algo_distribution dist, std::shared_ptr&lt; sopt::LinearTransform&lt; Vector&lt; typename Algorithm::Scalar &gt;&gt; const &gt; const &amp;measurements, std::shared_ptr&lt; sopt::LinearTransform&lt; Vector&lt; typename Algorithm::Scalar &gt;&gt; const &gt; const &amp;wavelets, const utilities::vis_params &amp;uv_data, const t_real sigma, const t_real step_size, const t_real reg_parameter, const t_uint imsizey, const t_uint imsizex, const t_uint sara_size, const t_uint max_iterations=500, const bool real_constraint=true, const bool positive_constraint=true, const bool tight_frame=false, const t_real relative_variation=1e-3, const t_real l1_proximal_tolerance=1e-2, const t_uint maximum_proximal_iterations=50)</arglist>
    </member>
    <member kind="variable">
      <type>const std::map&lt; std::string, algo_distribution &gt;</type>
      <name>algo_distribution_string</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>a27d858e0728daf0eff9998983c8c72a5</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>AllToAllSparseVector.cc</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/AllToAllSparseVector_8cc</filename>
    <includes id="AllToAllSparseVector_8h" name="AllToAllSparseVector.h" local="yes" imported="no">purify/AllToAllSparseVector.h</includes>
    <namespace>purify</namespace>
    <member kind="function">
      <type>std::vector&lt; t_int &gt;</type>
      <name>all_to_all_recv_sizes</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a85ac6ed0c897c6398e3a7d09c966238a</anchor>
      <arglist>(const std::vector&lt; t_int &gt; &amp;local_indices, const t_int nodes, const t_int N)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; t_int &gt;</type>
      <name>all_to_all_send_sizes</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>ad40efb8e8768a0bf074af57e89d2e64c</anchor>
      <arglist>(const std::vector&lt; t_int &gt; &amp;recv_sizes, const sopt::mpi::Communicator &amp;comm)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>AllToAllSparseVector.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/AllToAllSparseVector_8h</filename>
  </compound>
  <compound kind="file">
    <name>casacore.cc</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/purify_2casacore_8cc</filename>
    <includes id="casacore_8h" name="casacore.h" local="yes" imported="no">purify/casacore.h</includes>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::casa</namespace>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_measurementset</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>ab58d3422a795bff8b688ce650dc0b513</anchor>
      <arglist>(std::string const &amp;filename, const stokes pol=stokes::I, const std::vector&lt; t_int &gt; &amp;channels=std::vector&lt; t_int &gt;(), std::string const &amp;filter=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_measurementset</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>a18ac7690d95877e32386a1cdf20d24cd</anchor>
      <arglist>(std::string const &amp;filename, const utilities::vis_params &amp;uv1, const stokes pol, const std::vector&lt; t_int &gt; &amp;channels, std::string const &amp;filter)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_measurementset</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>a5cb78b815300138060a78e2e69610b12</anchor>
      <arglist>(std::vector&lt; std::string &gt; const &amp;filename, const stokes pol=stokes::I, const std::vector&lt; t_int &gt; &amp;channels=std::vector&lt; t_int &gt;(), std::string const &amp;filter=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_measurementset</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>a5a2e476c113ea88c281eb84e82deceb4</anchor>
      <arglist>(MeasurementSet const &amp;ms_file, const stokes pol=stokes::I, const std::vector&lt; t_int &gt; &amp;channels=std::vector&lt; t_int &gt;(), std::string const &amp;filter=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; utilities::vis_params &gt;</type>
      <name>read_measurementset_channels</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>aec0779879c336f74d45308d2333a8089</anchor>
      <arglist>(std::string const &amp;filename, const stokes pol=stokes::I, const t_int &amp;channel_width=1, std::string const &amp;filter=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>average_frequency</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>a401de6500811d6b4d4f6481975f3482d</anchor>
      <arglist>(const purify::casa::MeasurementSet &amp;ms_file, std::string const &amp;filter, const std::vector&lt; t_int &gt; &amp;channels)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>casacore.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/tests_2casacore_8cc</filename>
    <includes id="casacore_8h" name="casacore.h" local="yes" imported="no">purify/casacore.h</includes>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <class kind="class">TmpPath</class>
    <class kind="class">TmpMS</class>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2casacore_8cc.html</anchorfile>
      <anchor>aa6f87a321f9aefd0623ac7d8cd5630aa</anchor>
      <arglist>(&quot;Casacore&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2casacore_8cc.html</anchorfile>
      <anchor>aac6ba063eb31d5e89b1fc97563f28559</anchor>
      <arglist>(&quot;Size/Number of channels&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2casacore_8cc.html</anchorfile>
      <anchor>a249c6e9c961faa48e575ae9e314728ed</anchor>
      <arglist>(&quot;Single channel&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2casacore_8cc.html</anchorfile>
      <anchor>a81b05d0672024c94c85c4bac8a243721</anchor>
      <arglist>(&quot;Measurement channel&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2casacore_8cc.html</anchorfile>
      <anchor>ad5af2e85bf501c8ac6b7604509d5f50c</anchor>
      <arglist>(&quot;Channel iteration&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2casacore_8cc.html</anchorfile>
      <anchor>a7eed71d10b218b4f6095dad889bbd10d</anchor>
      <arglist>(&quot;Direction&quot;)</arglist>
    </member>
    <member kind="variable">
      <type>const std::string</type>
      <name>test_file</name>
      <anchorfile>tests_2casacore_8cc.html</anchorfile>
      <anchor>a3eed1c320ec34a19c4f87c0e5aa301ed</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>casacore.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/casacore_8h</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <class kind="class">purify::casa::MeasurementSet</class>
    <class kind="class">purify::casa::MeasurementSet::ChannelWrapper</class>
    <class kind="class">purify::casa::MeasurementSet::const_iterator</class>
    <namespace>purify</namespace>
    <namespace>purify::casa</namespace>
    <namespace>purify::casa::details</namespace>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_MACRO</name>
      <anchorfile>casacore_8h.html</anchorfile>
      <anchor>a4e0f9f56fd812ea59ad7a9448d7ff591</anchor>
      <arglist>(NAME, TYPE)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_MACRO</name>
      <anchorfile>casacore_8h.html</anchorfile>
      <anchor>a4e0f9f56fd812ea59ad7a9448d7ff591</anchor>
      <arglist>(NAME, TYPE)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_MACRO</name>
      <anchorfile>casacore_8h.html</anchorfile>
      <anchor>a4e0f9f56fd812ea59ad7a9448d7ff591</anchor>
      <arglist>(NAME, TYPE)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_MACRO</name>
      <anchorfile>casacore_8h.html</anchorfile>
      <anchor>a4e0f9f56fd812ea59ad7a9448d7ff591</anchor>
      <arglist>(NAME, TYPE)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_MACRO</name>
      <anchorfile>casacore_8h.html</anchorfile>
      <anchor>afdbb93ddd9d3f0f6942a6cf8f8527c7b</anchor>
      <arglist>(NAME, INDEX)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_MACRO</name>
      <anchorfile>casacore_8h.html</anchorfile>
      <anchor>ac186cc31869dcc887227c73c69aa9e8a</anchor>
      <arglist>(NAME)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; T &gt;</type>
      <name>table_column</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>a70ba290dc048b03efada7314b8ba5884</anchor>
      <arglist>(::casacore::Table const &amp;table, std::string const &amp;column, std::string const &amp;filter=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; C &gt;</type>
      <name>get_taql_array</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa_1_1details.html</anchorfile>
      <anchor>ae9df719c75711a86c94bc0486daa0de7</anchor>
      <arglist>(::casacore::TaQLResult const &amp;taql)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; C &gt;</type>
      <name>get_taql_scalar</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa_1_1details.html</anchorfile>
      <anchor>a4a16e38e7197aa89f4fa9302892a10ef</anchor>
      <arglist>(::casacore::TaQLResult const &amp;taql)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; T &gt;</type>
      <name>table_column</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa_1_1details.html</anchorfile>
      <anchor>aa097b6543eaa0e67a512c8a2b7a46fa9</anchor>
      <arglist>(::casacore::Table const &amp;table, std::string const &amp;column, std::string const &amp;filter, std::integral_constant&lt; bool, true &gt; const &amp;)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; T &gt;</type>
      <name>table_column</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa_1_1details.html</anchorfile>
      <anchor>ae6985d978f24bb26ad114a6504b8eaa0</anchor>
      <arglist>(::casacore::Table const &amp;table, std::string const &amp;column, std::string const &amp;filter, std::integral_constant&lt; bool, false &gt; const &amp;)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_measurementset</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>ab58d3422a795bff8b688ce650dc0b513</anchor>
      <arglist>(std::string const &amp;filename, const stokes pol=stokes::I, const std::vector&lt; t_int &gt; &amp;channels=std::vector&lt; t_int &gt;(), std::string const &amp;filter=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_measurementset</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>a5a2e476c113ea88c281eb84e82deceb4</anchor>
      <arglist>(MeasurementSet const &amp;ms_file, const stokes pol=stokes::I, const std::vector&lt; t_int &gt; &amp;channels=std::vector&lt; t_int &gt;(), std::string const &amp;filter=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_measurementset</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>a18ac7690d95877e32386a1cdf20d24cd</anchor>
      <arglist>(std::string const &amp;filename, const utilities::vis_params &amp;uv1, const stokes pol, const std::vector&lt; t_int &gt; &amp;channels, std::string const &amp;filter)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_measurementset</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>a5cb78b815300138060a78e2e69610b12</anchor>
      <arglist>(std::vector&lt; std::string &gt; const &amp;filename, const stokes pol=stokes::I, const std::vector&lt; t_int &gt; &amp;channels=std::vector&lt; t_int &gt;(), std::string const &amp;filter=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>average_frequency</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>a401de6500811d6b4d4f6481975f3482d</anchor>
      <arglist>(const purify::casa::MeasurementSet &amp;ms_file, std::string const &amp;filter, const std::vector&lt; t_int &gt; &amp;channels)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; utilities::vis_params &gt;</type>
      <name>read_measurementset_channels</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>aec0779879c336f74d45308d2333a8089</anchor>
      <arglist>(std::string const &amp;filename, const stokes pol=stokes::I, const t_int &amp;channel_width=1, std::string const &amp;filter=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>MeasurementSet::const_iterator</type>
      <name>operator+</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>a9fe7e9400d83c6b30f7c4ea09dd05be4</anchor>
      <arglist>(MeasurementSet::const_iterator::difference_type n, MeasurementSet::const_iterator const &amp;c)</arglist>
    </member>
    <member kind="function">
      <type>MeasurementSet::const_iterator</type>
      <name>operator-</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>a8b4b2cd370ccf6a6ce9c864c8d8292dd</anchor>
      <arglist>(MeasurementSet::const_iterator::difference_type n, MeasurementSet::const_iterator const &amp;c)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>cimg.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/cimg_8h</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
  </compound>
  <compound kind="file">
    <name>config.in.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/config_8in_8h</filename>
    <namespace>purify</namespace>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_HAS_USING</name>
      <anchorfile>config_8in_8h.html</anchorfile>
      <anchor>a8e4b643c64a8814f11d5f9b7776ece5a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_DO_LOGGING</name>
      <anchorfile>config_8in_8h.html</anchorfile>
      <anchor>acb4684f4c7657d334277f117d385a319</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_OPENMP</name>
      <anchorfile>config_8in_8h.html</anchorfile>
      <anchor>a031729dd3cdcae0065df1f53cec60859</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_OPENMP_FFTW</name>
      <anchorfile>config_8in_8h.html</anchorfile>
      <anchor>ae475c0b9995c7db8da07f01c56f931e9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_MPI</name>
      <anchorfile>config_8in_8h.html</anchorfile>
      <anchor>a1d4bb320f916525911aa7f03a7dab1ac</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_ARRAYFIRE</name>
      <anchorfile>config_8in_8h.html</anchorfile>
      <anchor>adbece49a101d6ae8524318d96ee522bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_CImg</name>
      <anchorfile>config_8in_8h.html</anchorfile>
      <anchor>a373d97ef824ffb387155468104405ccd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_CASACORE</name>
      <anchorfile>config_8in_8h.html</anchorfile>
      <anchor>a949747b2ae793d86aecd0f88cc927f5b</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>version</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a350eb23d7ebcdb7aae407717a7aad9d0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; uint8_t, uint8_t, uint8_t &gt;</type>
      <name>version_tuple</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>ac8f671077e81035877faca4f9bc5f25a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>gitref</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a5ccb5e37cec2a6be6fd26b42b0a66d79</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>default_logging_level</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a0108c76ec3e4a548088e9f5d5f6b4217</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>default_logger_name</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a4d58563620e972c279f841f66a8f68ef</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>constexpr bool</type>
      <name>color_logger</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a1719d0241f5cef19842ac9eb91272777</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>convergence_factory.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/convergence__factory_8h</filename>
    <namespace>purify</namespace>
    <namespace>purify::factory</namespace>
    <member kind="enumeration">
      <type></type>
      <name>ConvergenceType</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>ae3112c9c2dd6bbf06b1c59b4549f89c6</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="ae3112c9c2dd6bbf06b1c59b4549f89c6aec43da7d4be20024750a9c32d3236a9c">mpi_local</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="ae3112c9c2dd6bbf06b1c59b4549f89c6a23ddf3dd46824331a36d2d2b62e903fe">mpi_global</enumvalue>
    </member>
  </compound>
  <compound kind="file">
    <name>convolution.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/convolution_8h</filename>
    <namespace>purify</namespace>
    <namespace>purify::convol</namespace>
    <member kind="function">
      <type>Vector&lt; T &gt;</type>
      <name>zero_pad</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1convol.html</anchorfile>
      <anchor>a1e84eceae2f9c870a3e678f6bd6763f9</anchor>
      <arglist>(const Vector&lt; T &gt; &amp;input, const t_int padding)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; T &gt;</type>
      <name>zero_pad</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1convol.html</anchorfile>
      <anchor>a4ae340f99f2c6cc64b316f8394382fbc</anchor>
      <arglist>(const Matrix&lt; T &gt; &amp;input, const t_int paddingv, const t_int paddingu)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; T &gt;</type>
      <name>linear_convol_1d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1convol.html</anchorfile>
      <anchor>adf71b9fa393f32b36d0cbc545d831360</anchor>
      <arglist>(const Vector&lt; T &gt; &amp;kernelf, const Vector&lt; T &gt; &amp;kernelg)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; T &gt;</type>
      <name>linear_convol_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1convol.html</anchorfile>
      <anchor>a8243104c62ca603ba048c521ca99be92</anchor>
      <arglist>(const Vector&lt; T &gt; &amp;kernelfu, const Vector&lt; T &gt; &amp;kernelfv, const Matrix&lt; T &gt; &amp;kernelg)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; T &gt;</type>
      <name>linear_convol_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1convol.html</anchorfile>
      <anchor>a8d09d81178acd9e2b707396697b39c1a</anchor>
      <arglist>(const std::function&lt; T(t_int)&gt; &amp;kernelu, const std::function&lt; T(t_int)&gt; &amp;kernelv, const std::function&lt; T(t_int, t_int)&gt; &amp;kernelw, const t_uint &amp;Jfu, const t_uint &amp;Jfv, const t_uint &amp;Jgu, const t_uint &amp;Jgv)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>distribute.cc</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/purify_2distribute_8cc</filename>
    <includes id="distribute_8h" name="distribute.h" local="yes" imported="no">purify/distribute.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::distribute</namespace>
    <member kind="function">
      <type>std::vector&lt; t_int &gt;</type>
      <name>distribute_measurements</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</anchorfile>
      <anchor>ac92e70bdd1349f71c586e2ae418aacac</anchor>
      <arglist>(Vector&lt; t_real &gt; const &amp;u, Vector&lt; t_real &gt; const &amp;v, Vector&lt; t_real &gt; const &amp;w, t_int const number_of_nodes, distribute::plan const distribution_plan=plan::equal, t_int const &amp;grid_size=128)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_int &gt;</type>
      <name>w_distribution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</anchorfile>
      <anchor>a4d4883a51a1bcc624ea4c2140e4b322d</anchor>
      <arglist>(Vector&lt; t_real &gt; const &amp;u, const Vector&lt; t_real &gt; &amp;v, const Vector&lt; t_real &gt; &amp;w)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_int &gt;</type>
      <name>w_distribution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</anchorfile>
      <anchor>aa84ba162545cce84263919102c2ada05</anchor>
      <arglist>(Vector&lt; t_real &gt; const &amp;w)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_int &gt;</type>
      <name>distance_distribution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</anchorfile>
      <anchor>afa3ab873e06dd32e3f2ce5e968900b93</anchor>
      <arglist>(Vector&lt; t_real &gt; const &amp;u, Vector&lt; t_real &gt; const &amp;v)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_int &gt;</type>
      <name>equal_distribution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</anchorfile>
      <anchor>a7c0aef9bb156212fd92deaaa148510fc</anchor>
      <arglist>(Vector&lt; t_real &gt; const &amp;u, Vector&lt; t_real &gt; const &amp;v, t_int const &amp;grid_size)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; std::vector&lt; t_int &gt;, std::vector&lt; t_real &gt; &gt;</type>
      <name>kmeans_algo</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</anchorfile>
      <anchor>ae6155f9cc952e699bb3b40941a166171</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;w, const t_int number_of_nodes, const t_int iters, const std::function&lt; t_real(t_real)&gt; &amp;cost=[](t_real x) { return x *x;})</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>distribute.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/tests_2distribute_8cc</filename>
    <includes id="distribute_8h" name="distribute.h" local="yes" imported="no">purify/distribute.h</includes>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2distribute_8cc.html</anchorfile>
      <anchor>aaaca9e8c7fef2479cd6e724d3df71136</anchor>
      <arglist>(&quot;Distribute&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>distribute.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/distribute_8h</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::distribute</namespace>
    <member kind="enumeration">
      <type></type>
      <name>plan</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</anchorfile>
      <anchor>a0bf24e0d0fb613140687838e4546ab2a</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html" anchor="a0bf24e0d0fb613140687838e4546ab2aa334c4a4c42fdb79d7ebc3e73b517e6f8">none</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html" anchor="a0bf24e0d0fb613140687838e4546ab2aa465289687a70db7aa7217cc240c29f0f">equal</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html" anchor="a0bf24e0d0fb613140687838e4546ab2aa869836a44b607137a7e094cf8a308938">radial</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html" anchor="a0bf24e0d0fb613140687838e4546ab2aa40b63143a987ddcf3aa69fb2ac32a3bd">w_term</enumvalue>
    </member>
    <member kind="function">
      <type>std::vector&lt; t_int &gt;</type>
      <name>distribute_measurements</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</anchorfile>
      <anchor>ac92e70bdd1349f71c586e2ae418aacac</anchor>
      <arglist>(Vector&lt; t_real &gt; const &amp;u, Vector&lt; t_real &gt; const &amp;v, Vector&lt; t_real &gt; const &amp;w, t_int const number_of_nodes, distribute::plan const distribution_plan=plan::equal, t_int const &amp;grid_size=128)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; std::vector&lt; t_int &gt;, std::vector&lt; t_real &gt; &gt;</type>
      <name>kmeans_algo</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</anchorfile>
      <anchor>ae6155f9cc952e699bb3b40941a166171</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;w, const t_int number_of_nodes, const t_int iters, const std::function&lt; t_real(t_real)&gt; &amp;cost=[](t_real x) { return x *x;})</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_int &gt;</type>
      <name>w_distribution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</anchorfile>
      <anchor>a4d4883a51a1bcc624ea4c2140e4b322d</anchor>
      <arglist>(Vector&lt; t_real &gt; const &amp;u, const Vector&lt; t_real &gt; &amp;v, const Vector&lt; t_real &gt; &amp;w)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_int &gt;</type>
      <name>w_distribution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</anchorfile>
      <anchor>aa84ba162545cce84263919102c2ada05</anchor>
      <arglist>(Vector&lt; t_real &gt; const &amp;w)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_int &gt;</type>
      <name>distance_distribution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</anchorfile>
      <anchor>afa3ab873e06dd32e3f2ce5e968900b93</anchor>
      <arglist>(Vector&lt; t_real &gt; const &amp;u, Vector&lt; t_real &gt; const &amp;v)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_int &gt;</type>
      <name>equal_distribution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</anchorfile>
      <anchor>a7c0aef9bb156212fd92deaaa148510fc</anchor>
      <arglist>(Vector&lt; t_real &gt; const &amp;u, Vector&lt; t_real &gt; const &amp;v, t_int const &amp;grid_size)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>DistributeSparseVector.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/DistributeSparseVector_8h</filename>
  </compound>
  <compound kind="file">
    <name>IndexMapping.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/IndexMapping_8h</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <class kind="class">purify::IndexMapping</class>
    <namespace>purify</namespace>
    <member kind="function">
      <type>std::set&lt; t_int &gt;</type>
      <name>non_empty_outers</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>af9469839053b3fb67fdc9d0df0c35eba</anchor>
      <arglist>(Eigen::SparseMatrixBase&lt; T0 &gt; const &amp;matrix)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; typename T0::Scalar &gt;</type>
      <name>compress_outer</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>ae5a8e573961b3d00cc5603cf4e5e647f</anchor>
      <arglist>(T0 const &amp;matrix)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>integration.cc</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/purify_2integration_8cc</filename>
    <includes id="integration_8h" name="integration.h" local="yes" imported="no">purify/integration.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::integration</namespace>
    <member kind="function">
      <type>t_real</type>
      <name>integrate</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>ae71083362428137f328d85469be8c255</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;xmin, const Vector&lt; t_real &gt; &amp;xmax, const std::function&lt; t_real(Vector&lt; t_real &gt;)&gt; &amp;func, const norm_type norm, const t_real required_abs_error, const t_real required_rel_error, const t_uint max_evaluations, const method methodtype)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>integrate</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>a10cce957449a468b764cf02c240848ac</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;xmin, const Vector&lt; t_real &gt; &amp;xmax, const std::function&lt; t_complex(Vector&lt; t_real &gt;)&gt; &amp;func, const norm_type norm, const t_real required_abs_error, const t_real required_rel_error, const t_uint max_evaluations, const method methodtype)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_real &gt;</type>
      <name>integrate</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>a7d5e16597c855bdf29d195364c353e18</anchor>
      <arglist>(const t_uint fdim, const Vector&lt; t_real &gt; &amp;xmin, const Vector&lt; t_real &gt; &amp;xmax, const std::function&lt; Vector&lt; t_real &gt;(Vector&lt; t_real &gt;)&gt; &amp;func, const norm_type norm, const t_real required_abs_error, const t_real required_rel_error, const t_uint max_evaluations, const method methodtype)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_complex &gt;</type>
      <name>integrate_v</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>a1cd627283e9e7bca5d0b763a9f7b05c5</anchor>
      <arglist>(const t_uint fdim, const t_uint npts, const Vector&lt; t_real &gt; &amp;xmin, const Vector&lt; t_real &gt; &amp;xmax, const std::vector&lt; std::function&lt; t_complex(Vector&lt; t_real &gt;)&gt;&gt; &amp;func, const norm_type norm, const t_real required_abs_error, const t_real required_rel_error, const t_uint max_evaluations, const method methodtype)</arglist>
    </member>
    <member kind="function">
      <type>error_norm</type>
      <name>norm_error</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>ad77278105245f570120de72b95222a08</anchor>
      <arglist>(norm_type norm)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>convolution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>a42f348e8892ab417a52e17adc46f549d</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;x0, const Vector&lt; t_real &gt; &amp;xmin, const Vector&lt; t_real &gt; &amp;xmax, const std::function&lt; t_complex(Vector&lt; t_real &gt;)&gt; &amp;func1, const std::function&lt; t_complex(Vector&lt; t_real &gt;)&gt; &amp;func2, const norm_type norm, const t_real required_abs_error, const t_real required_rel_error, const t_uint max_evaluations, const method methodtype)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>integration.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/tests_2integration_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="integration_8h" name="integration.h" local="yes" imported="no">purify/integration.h</includes>
    <includes id="kernels_8h" name="kernels.h" local="yes" imported="no">purify/kernels.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2integration_8cc.html</anchorfile>
      <anchor>afc4d43e2965e42f4a9ec6ed7f563d4e5</anchor>
      <arglist>(&quot;integration&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2integration_8cc.html</anchorfile>
      <anchor>a73fd2cafef4d76d864fb2919138032c6</anchor>
      <arglist>(&quot;complex&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2integration_8cc.html</anchorfile>
      <anchor>a09232d6b87cd69d0cbd1399674366d81</anchor>
      <arglist>(&quot;Numerical_Fourier_transform&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>integration.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/integration_8h</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::integration</namespace>
    <member kind="enumeration">
      <type></type>
      <name>norm_type</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>a5d8cd1673befa0bfd8cc7618ff5a9816</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html" anchor="a5d8cd1673befa0bfd8cc7618ff5a9816a23b79ae0fc0f07a3669598dd23c694cc">individual</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html" anchor="a5d8cd1673befa0bfd8cc7618ff5a9816ab738418ba05b7e56726509f09f725672">paired</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html" anchor="a5d8cd1673befa0bfd8cc7618ff5a9816a377fd569971eedeba8fbea28434a390a">l1</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html" anchor="a5d8cd1673befa0bfd8cc7618ff5a9816abec25675775e9e0a0d783a5018b463e3">l2</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html" anchor="a5d8cd1673befa0bfd8cc7618ff5a9816a707f3c5e9c59c9e5f347d2cbec59c66b">linf</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>method</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>a29ad3e35748f2774b6fe751c8f9a4cc9</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html" anchor="a29ad3e35748f2774b6fe751c8f9a4cc9a2510c39011c5be704182423e3a695e91">h</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html" anchor="a29ad3e35748f2774b6fe751c8f9a4cc9a83878c91171338902e0fe0fb97a8c47a">p</enumvalue>
    </member>
    <member kind="function">
      <type>error_norm</type>
      <name>norm_error</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>ad77278105245f570120de72b95222a08</anchor>
      <arglist>(norm_type norm)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>integrate</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>ae71083362428137f328d85469be8c255</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;xmin, const Vector&lt; t_real &gt; &amp;xmax, const std::function&lt; t_real(Vector&lt; t_real &gt;)&gt; &amp;func, const norm_type norm, const t_real required_abs_error, const t_real required_rel_error, const t_uint max_evaluations, const method methodtype)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>integrate</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>a10cce957449a468b764cf02c240848ac</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;xmin, const Vector&lt; t_real &gt; &amp;xmax, const std::function&lt; t_complex(Vector&lt; t_real &gt;)&gt; &amp;func, const norm_type norm, const t_real required_abs_error, const t_real required_rel_error, const t_uint max_evaluations, const method methodtype)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_real &gt;</type>
      <name>integrate</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>a7d5e16597c855bdf29d195364c353e18</anchor>
      <arglist>(const t_uint fdim, const Vector&lt; t_real &gt; &amp;xmin, const Vector&lt; t_real &gt; &amp;xmax, const std::function&lt; Vector&lt; t_real &gt;(Vector&lt; t_real &gt;)&gt; &amp;func, const norm_type norm, const t_real required_abs_error, const t_real required_rel_error, const t_uint max_evaluations, const method methodtype)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_complex &gt;</type>
      <name>integrate_v</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>af0570a8b8956046214f724b5e6982344</anchor>
      <arglist>(const t_uint fdim, const Vector&lt; t_real &gt; &amp;xmin, const Vector&lt; t_real &gt; &amp;xmax, const std::vector&lt; std::function&lt; t_complex(Vector&lt; t_real &gt;)&gt;&gt; &amp;func, const norm_type norm, const t_real required_abs_error, const t_real required_rel_error, const t_uint max_evaluations, const method methodtype)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>convolution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>a42f348e8892ab417a52e17adc46f549d</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;x0, const Vector&lt; t_real &gt; &amp;xmin, const Vector&lt; t_real &gt; &amp;xmax, const std::function&lt; t_complex(Vector&lt; t_real &gt;)&gt; &amp;func1, const std::function&lt; t_complex(Vector&lt; t_real &gt;)&gt; &amp;func2, const norm_type norm, const t_real required_abs_error, const t_real required_rel_error, const t_uint max_evaluations, const method methodtype)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>convolution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>a89922f501faa29aeee1ddee469d7382c</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;x0, const std::function&lt; t_complex(Vector&lt; t_real &gt;)&gt; &amp;func1, const std::function&lt; t_complex(Vector&lt; t_real &gt;)&gt; &amp;func2, const norm_type norm, const t_real required_abs_error, const t_real required_rel_error, const t_uint max_evaluations, const method methodtype)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>kernels.cc</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/purify_2kernels_8cc</filename>
    <includes id="kernels_8h" name="kernels.h" local="yes" imported="no">purify/kernels.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::kernels</namespace>
    <member kind="function">
      <type>t_real</type>
      <name>kaiser_bessel</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a5ebd5a33dc46d12eca370fb011c6d20a</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>kaiser_bessel_general</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a95321ed82db7eb7e34a706bf0de8135a</anchor>
      <arglist>(const t_real x, const t_real J, const t_real alpha)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>ft_kaiser_bessel_general</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>afdc7151c592c299ad32241b9af5465e9</anchor>
      <arglist>(const t_real x, const t_real J, const t_real alpha)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>ft_kaiser_bessel</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a2703c3d44226204c45f3219cc890edef</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>gaussian</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>aca4681b624d0137ad6a594cf93393015</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>ft_gaussian</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>afd78fa880a5c40cc7d98d741efedea1d</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>calc_for_pswf</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a9ead940a0dfa9fcbc121c11a88e0b3dd</anchor>
      <arglist>(const t_real x, const t_real J, const t_real alpha)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>pswf</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>af392bdc1af64b376a17cf66e8a7f911a</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>ft_pswf</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a8c6fa490c8163e98471c720580b71c49</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_real &gt;</type>
      <name>kernel_samples</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a47eee0820c6469891800858427889494</anchor>
      <arglist>(const t_real total_samples, const std::function&lt; t_real(t_real)&gt; kernelu, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>kernel_linear_interp</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>add23bd0781527fd3fb7adfbdcd540c90</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;samples, const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>pill_box</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a7940b1b5055b0ada972997661383c30a</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>ft_pill_box</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>abe1d622ce6528a7ca83d509943503d22</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>gaussian_general</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a9144b80de7e992f02fa42d2f2e4557b7</anchor>
      <arglist>(const t_real x, const t_real J, const t_real sigma)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>ft_gaussian_general</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a539e03adfcf4e7bc963e8282a722940e</anchor>
      <arglist>(const t_real x, const t_real J, const t_real sigma)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; std::function&lt; t_real(t_real)&gt;, std::function&lt; t_real(t_real)&gt;, std::function&lt; t_real(t_real)&gt;, std::function&lt; t_real(t_real)&gt; &gt;</type>
      <name>create_kernels</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a667d00cb4a0af96a8904a97f83797e7e</anchor>
      <arglist>(const kernels::kernel kernel_name_, const t_uint Ju_, const t_uint Jv_, const t_real imsizey_, const t_real imsizex_, const t_real oversample_ratio)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; std::function&lt; t_real(t_real)&gt;, std::function&lt; t_real(t_real)&gt; &gt;</type>
      <name>create_radial_ftkernel</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a3842f9866b28e4e47a59f78222b89b5f</anchor>
      <arglist>(const kernels::kernel kernel_name_, const t_uint Ju_, const t_real oversample_ratio)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>kernels.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/tests_2kernels_8cc</filename>
    <includes id="kernels_8h" name="kernels.h" local="yes" imported="no">purify/kernels.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <namespace>kernels_test</namespace>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2kernels_8cc.html</anchorfile>
      <anchor>a0b293e73f7e3f927870ad68b2ab02cdf</anchor>
      <arglist>(&quot;kernels&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2kernels_8cc.html</anchorfile>
      <anchor>af95ef159e85bbfd3715e6ea7aa899be9</anchor>
      <arglist>(&quot;kernel_correction&quot;)</arglist>
    </member>
    <member kind="variable">
      <type>const std::string</type>
      <name>test_dir</name>
      <anchorfile>namespacekernels__test.html</anchorfile>
      <anchor>afe7cd15fa169e6cab3e7a372d8ec882f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_real &gt;</type>
      <name>kernel_x_values</name>
      <anchorfile>namespacekernels__test.html</anchorfile>
      <anchor>abfe98f98ba03415a8e5d0dab22daf9b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_real &gt;</type>
      <name>kernel_pswf_values</name>
      <anchorfile>namespacekernels__test.html</anchorfile>
      <anchor>a7bb2fa229d396344b77fb114572058be</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_real &gt;</type>
      <name>kernel_kb_values</name>
      <anchorfile>namespacekernels__test.html</anchorfile>
      <anchor>abd51c1413c5c3da8244ec2aafa8c783f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_real &gt;</type>
      <name>kernel_gauss_values</name>
      <anchorfile>namespacekernels__test.html</anchorfile>
      <anchor>a0f906705aecd45568868ad81da26718c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_real &gt;</type>
      <name>correction_x_values</name>
      <anchorfile>namespacekernels__test.html</anchorfile>
      <anchor>ab5287a8320808f65ad59df31c593fa94</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_real &gt;</type>
      <name>correction_pswf_values</name>
      <anchorfile>namespacekernels__test.html</anchorfile>
      <anchor>a5ee213c2abf84def1dcf73447babb4b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_real &gt;</type>
      <name>correction_kb_values</name>
      <anchorfile>namespacekernels__test.html</anchorfile>
      <anchor>aeb51f1ab3a64e893a51328e52b874a9e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_real &gt;</type>
      <name>correction_gauss_values</name>
      <anchorfile>namespacekernels__test.html</anchorfile>
      <anchor>a8c014762dbb7842cd038bb3c29a9178c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>kernels.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/kernels_8h</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::kernels</namespace>
    <member kind="enumeration">
      <type></type>
      <name>kernel</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a5d1a602d4c5e183ca2ea9bf831d61668</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html" anchor="a5d1a602d4c5e183ca2ea9bf831d61668aba34ea40525a4379add785228e37fe86">kb</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html" anchor="a5d1a602d4c5e183ca2ea9bf831d61668a710a4950250286365cf841f765a790f1">gauss</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html" anchor="a5d1a602d4c5e183ca2ea9bf831d61668a34be958a921e43d813a2075297d8e862">box</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html" anchor="a5d1a602d4c5e183ca2ea9bf831d61668a214d868f9c3f802c756980667a30cc65">pswf</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html" anchor="a5d1a602d4c5e183ca2ea9bf831d61668ac8f9b2014e544adacfc19dfeba3718e8">kbmin</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html" anchor="a5d1a602d4c5e183ca2ea9bf831d61668a575e9f12266a1af6d0b1cce8da8bc310">gauss_alt</enumvalue>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>kaiser_bessel</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a5ebd5a33dc46d12eca370fb011c6d20a</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>kaiser_bessel_general</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a95321ed82db7eb7e34a706bf0de8135a</anchor>
      <arglist>(const t_real x, const t_real J, const t_real alpha)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>ft_kaiser_bessel_general</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>afdc7151c592c299ad32241b9af5465e9</anchor>
      <arglist>(const t_real x, const t_real J, const t_real alpha)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>ft_kaiser_bessel</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a2703c3d44226204c45f3219cc890edef</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>gaussian</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>aca4681b624d0137ad6a594cf93393015</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>ft_gaussian</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>afd78fa880a5c40cc7d98d741efedea1d</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>calc_for_pswf</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a9ead940a0dfa9fcbc121c11a88e0b3dd</anchor>
      <arglist>(const t_real x, const t_real J, const t_real alpha)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>pswf</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>af392bdc1af64b376a17cf66e8a7f911a</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>ft_pswf</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a8c6fa490c8163e98471c720580b71c49</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_real &gt;</type>
      <name>kernel_samples</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a47eee0820c6469891800858427889494</anchor>
      <arglist>(const t_real total_samples, const std::function&lt; t_real(t_real)&gt; kernelu, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>kernel_linear_interp</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>add23bd0781527fd3fb7adfbdcd540c90</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;samples, const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>pill_box</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a7940b1b5055b0ada972997661383c30a</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>ft_pill_box</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>abe1d622ce6528a7ca83d509943503d22</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>gaussian_general</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a9144b80de7e992f02fa42d2f2e4557b7</anchor>
      <arglist>(const t_real x, const t_real J, const t_real sigma)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>ft_gaussian_general</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a539e03adfcf4e7bc963e8282a722940e</anchor>
      <arglist>(const t_real x, const t_real J, const t_real sigma)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; std::function&lt; t_real(t_real)&gt;, std::function&lt; t_real(t_real)&gt;, std::function&lt; t_real(t_real)&gt;, std::function&lt; t_real(t_real)&gt; &gt;</type>
      <name>create_kernels</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a667d00cb4a0af96a8904a97f83797e7e</anchor>
      <arglist>(const kernels::kernel kernel_name_, const t_uint Ju_, const t_uint Jv_, const t_real imsizey_, const t_real imsizex_, const t_real oversample_ratio)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; std::function&lt; t_real(t_real)&gt;, std::function&lt; t_real(t_real)&gt; &gt;</type>
      <name>create_radial_ftkernel</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a3842f9866b28e4e47a59f78222b89b5f</anchor>
      <arglist>(const kernels::kernel kernel_name_, const t_uint Ju_, const t_real oversample_ratio)</arglist>
    </member>
    <member kind="variable">
      <type>const std::map&lt; std::string, kernel &gt;</type>
      <name>kernel_from_string</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a1e2f3c37e5c0c6e2f2a3282110891f31</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>logging.disabled.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/logging_8disabled_8h</filename>
    <namespace>purify</namespace>
    <namespace>purify::logging</namespace>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_LOG_</name>
      <anchorfile>logging_8disabled_8h.html</anchorfile>
      <anchor>a926705337510447c4e4c8ef1ca347613</anchor>
      <arglist>(...)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; int &gt;</type>
      <name>initialize</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</anchorfile>
      <anchor>a766221e3a05f736017082766ede24d4a</anchor>
      <arglist>(std::string const &amp;name=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; int &gt;</type>
      <name>initialize</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</anchorfile>
      <anchor>a5a761be31c4fb31b3a9e636c17133904</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; int &gt;</type>
      <name>get</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</anchorfile>
      <anchor>a96517fef9cb15e2b3cff0235f12f4d0a</anchor>
      <arglist>(std::string const &amp;name=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; int &gt;</type>
      <name>get</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</anchorfile>
      <anchor>a02a987cbc0c3d7d4c26190b547053998</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_level</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</anchorfile>
      <anchor>a46c69647ca4c2d4fe100e50884d6b442</anchor>
      <arglist>(std::string const &amp;level, std::string const &amp;name)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_level</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</anchorfile>
      <anchor>a9bada47c3aae602e8e246a164d79bada</anchor>
      <arglist>(std::string const &amp;)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>has_level</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</anchorfile>
      <anchor>a793bccd5a5042f11480fedfcbdcb56b8</anchor>
      <arglist>(std::string const &amp;, std::string const &amp;)</arglist>
    </member>
    <member kind="variable">
      <type>const std::string</type>
      <name>name_prefix</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</anchorfile>
      <anchor>ade846bfa55625b2ef77802f37adc2c86</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>logging.enabled.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/logging_8enabled_8h</filename>
    <namespace>purify</namespace>
    <namespace>purify::logging</namespace>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_MACRO</name>
      <anchorfile>logging_8enabled_8h.html</anchorfile>
      <anchor>ab91dfc81d34a40e86f5a27aca928d5fb</anchor>
      <arglist>(LEVEL)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_MACRO</name>
      <anchorfile>logging_8enabled_8h.html</anchorfile>
      <anchor>ab91dfc81d34a40e86f5a27aca928d5fb</anchor>
      <arglist>(LEVEL)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_LOG_</name>
      <anchorfile>logging_8enabled_8h.html</anchorfile>
      <anchor>ac8a4b465c53961c6c4a7980fee9f96c6</anchor>
      <arglist>(NAME, TYPE,...)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_level</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</anchorfile>
      <anchor>a46c69647ca4c2d4fe100e50884d6b442</anchor>
      <arglist>(std::string const &amp;level, std::string const &amp;name)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; int &gt;</type>
      <name>initialize</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</anchorfile>
      <anchor>a766221e3a05f736017082766ede24d4a</anchor>
      <arglist>(std::string const &amp;name=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; int &gt;</type>
      <name>get</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</anchorfile>
      <anchor>a96517fef9cb15e2b3cff0235f12f4d0a</anchor>
      <arglist>(std::string const &amp;name=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>has_level</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</anchorfile>
      <anchor>a793bccd5a5042f11480fedfcbdcb56b8</anchor>
      <arglist>(std::string const &amp;, std::string const &amp;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>logging.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/logging_8h</filename>
    <includes id="logging_8disabled_8h" name="logging.disabled.h" local="yes" imported="no">purify/logging.disabled.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_CRITICAL</name>
      <anchorfile>logging_8h.html</anchorfile>
      <anchor>a8137be11add63e96326a1b6c18a1bf43</anchor>
      <arglist>(...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_ERROR</name>
      <anchorfile>logging_8h.html</anchorfile>
      <anchor>a8436006ba02587ee29d4e22975394895</anchor>
      <arglist>(...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_WARN</name>
      <anchorfile>logging_8h.html</anchorfile>
      <anchor>a415aa1970a79c89dfda68d3ad9583aa7</anchor>
      <arglist>(...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_INFO</name>
      <anchorfile>logging_8h.html</anchorfile>
      <anchor>ade89364ee576bc12d1a4f3553e507f40</anchor>
      <arglist>(...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_DEBUG</name>
      <anchorfile>logging_8h.html</anchorfile>
      <anchor>a8aac71815ec42074b1410fa5ea48a268</anchor>
      <arglist>(...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_TRACE</name>
      <anchorfile>logging_8h.html</anchorfile>
      <anchor>aa26a839bf9a37459472d1ac6fa8203be</anchor>
      <arglist>(...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_HIGH_LOG</name>
      <anchorfile>logging_8h.html</anchorfile>
      <anchor>a4c0fef90efd9093d28b6dc8e10cb577d</anchor>
      <arglist>(...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_MEDIUM_LOG</name>
      <anchorfile>logging_8h.html</anchorfile>
      <anchor>ad2207dcfa3c2c44fa0fccbb9d0a3b21c</anchor>
      <arglist>(...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_LOW_LOG</name>
      <anchorfile>logging_8h.html</anchorfile>
      <anchor>a0c01897a385e812973d29bead7548584</anchor>
      <arglist>(...)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>measurement_operator_factory.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/measurement__operator__factory_8h</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="operators__gpu_8h" name="operators_gpu.h" local="yes" imported="no">purify/operators_gpu.h</includes>
    <includes id="wproj__operators_8h" name="wproj_operators.h" local="yes" imported="no">purify/wproj_operators.h</includes>
    <includes id="wproj__operators__gpu_8h" name="wproj_operators_gpu.h" local="yes" imported="no">purify/wproj_operators_gpu.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::factory</namespace>
    <member kind="enumeration">
      <type></type>
      <name>distributed_measurement_operator</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>a6b8de5116c3d7d0d679cd6dd47a80248</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a6b8de5116c3d7d0d679cd6dd47a80248a74136b24217cb75599440e6e1d807cf7">serial</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a6b8de5116c3d7d0d679cd6dd47a80248a3c25e5e5347476170daf56c780247dab">mpi_distribute_image</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a6b8de5116c3d7d0d679cd6dd47a80248ade789d500028594f8485e34f4fd201da">mpi_distribute_grid</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a6b8de5116c3d7d0d679cd6dd47a80248aeb435348cd1dd3b23bc83af306c8af4e">gpu_serial</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a6b8de5116c3d7d0d679cd6dd47a80248a3c5924386fd8ce3e082a2e4e2f73a3a2">gpu_mpi_distribute_image</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a6b8de5116c3d7d0d679cd6dd47a80248a91443239f9c1344dd49541bb99dfb3e0">gpu_mpi_distribute_grid</enumvalue>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; sopt::LinearTransform&lt; T &gt; &gt;</type>
      <name>measurement_operator_factory</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>acb403f80232cc471d467fe92d5802c2d</anchor>
      <arglist>(const distributed_measurement_operator distribute, ARGS &amp;&amp;... args)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>mpi_utilities.cc</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/purify_2mpi__utilities_8cc</filename>
    <includes id="mpi__utilities_8h" name="mpi_utilities.h" local="yes" imported="no">purify/mpi_utilities.h</includes>
    <includes id="distribute_8h" name="distribute.h" local="yes" imported="no">purify/distribute.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::utilities</namespace>
    <member kind="function">
      <type>void</type>
      <name>regroup</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a4a87838cfac5f5f656a75e0cdd33693b</anchor>
      <arglist>(vis_params &amp;uv_params, std::vector&lt; t_int &gt; const &amp;groups_, const t_int max_groups)</arglist>
    </member>
    <member kind="function">
      <type>vis_params</type>
      <name>regroup_and_scatter</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>acd42c5df2679743eb61058c3be5eaf01</anchor>
      <arglist>(vis_params const &amp;params, std::vector&lt; t_int &gt; const &amp;groups, sopt::mpi::Communicator const &amp;comm)</arglist>
    </member>
    <member kind="function">
      <type>vis_params</type>
      <name>regroup_and_all_to_all</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ac2aafb0f04fe070f18ff3b4afe66a4d8</anchor>
      <arglist>(vis_params const &amp;params, std::vector&lt; t_int &gt; const &amp;groups, sopt::mpi::Communicator const &amp;comm)</arglist>
    </member>
    <member kind="function">
      <type>vis_params</type>
      <name>all_to_all_visibilities</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a8e83fa8e3018614508a2a88e689bd260</anchor>
      <arglist>(vis_params const &amp;params, std::vector&lt; t_int &gt; const &amp;sizes, sopt::mpi::Communicator const &amp;comm)</arglist>
    </member>
    <member kind="function">
      <type>vis_params</type>
      <name>scatter_visibilities</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>abdce09971a0be5d91b6db1f978865710</anchor>
      <arglist>(vis_params const &amp;params, std::vector&lt; t_int &gt; const &amp;sizes, sopt::mpi::Communicator const &amp;comm)</arglist>
    </member>
    <member kind="function">
      <type>vis_params</type>
      <name>scatter_visibilities</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a787800823820f9480ef13458b3496aa7</anchor>
      <arglist>(sopt::mpi::Communicator const &amp;comm)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>distribute_params</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a2120ca26a6a6f0797051d7701d96ad14</anchor>
      <arglist>(utilities::vis_params const &amp;params, sopt::mpi::Communicator const &amp;comm)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>set_cell_size</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ae46ed33e75ac4e9921e1dd9ac8f8f4f0</anchor>
      <arglist>(const sopt::mpi::Communicator &amp;comm, utilities::vis_params const &amp;uv_vis, const t_real &amp;cell_x, const t_real &amp;cell_y)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>w_stacking</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>af19b6e8e20f56ac162dbbb8ad8136c51</anchor>
      <arglist>(utilities::vis_params const &amp;params, sopt::mpi::Communicator const &amp;comm, const t_int iters, const std::function&lt; t_real(t_real)&gt; &amp;cost)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>mpi_utilities.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/tests_2mpi__utilities_8cc</filename>
    <includes id="mpi__utilities_8h" name="mpi_utilities.h" local="yes" imported="no">purify/mpi_utilities.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2mpi__utilities_8cc.html</anchorfile>
      <anchor>ac2c7ba3c65cbfb134f774d61dd1a4210</anchor>
      <arglist>(&quot;re-group visibilities in order of processes&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>mpi_utilities.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/mpi__utilities_8h</filename>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::utilities</namespace>
    <member kind="function">
      <type>t_real</type>
      <name>step_size</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a49598f35180a53cbfef9c5d44dad7ec0</anchor>
      <arglist>(T const &amp;vis, const std::shared_ptr&lt; sopt::LinearTransform&lt; T &gt; const &gt; &amp;measurements, const std::shared_ptr&lt; sopt::LinearTransform&lt; T &gt; const &gt; &amp;wavelets, const t_uint sara_size)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>operators.cc</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/purify_2operators_8cc</filename>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="wide__field__utilities_8h" name="wide_field_utilities.h" local="yes" imported="no">purify/wide_field_utilities.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::details</namespace>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>init_gridding_matrix_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1details.html</anchorfile>
      <anchor>a96036c7a586e56d378dbbb4dad742144</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;u, const Vector&lt; t_real &gt; &amp;v, const Vector&lt; t_complex &gt; &amp;weights, const t_uint &amp;imsizey_, const t_uint &amp;imsizex_, const t_real &amp;oversample_ratio, const std::function&lt; t_real(t_real)&gt; kernelu, const std::function&lt; t_real(t_real)&gt; kernelv, const t_uint Ju, const t_uint Jv)</arglist>
    </member>
    <member kind="function">
      <type>Image&lt; t_complex &gt;</type>
      <name>init_correction2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1details.html</anchorfile>
      <anchor>a288d7453cd72dededadfb96cfcad60b4</anchor>
      <arglist>(const t_real &amp;oversample_ratio, const t_uint &amp;imsizey_, const t_uint &amp;imsizex_, const std::function&lt; t_real(t_real)&gt; ftkernelu, const std::function&lt; t_real(t_real)&gt; ftkernelv, const t_real &amp;w_mean, const t_real &amp;cellx, const t_real &amp;celly)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>operators.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/tests_2operators_8cc</filename>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="kernels_8h" name="kernels.h" local="yes" imported="no">purify/kernels.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <namespace>operators_test</namespace>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2operators_8cc.html</anchorfile>
      <anchor>a3b57e179b9eee3862f82554801486322</anchor>
      <arglist>(&quot;Operators&quot;)</arglist>
    </member>
    <member kind="variable">
      <type>const std::string</type>
      <name>test_dir</name>
      <anchorfile>namespaceoperators__test.html</anchorfile>
      <anchor>a165b68aac2b309ea97f327ee188141d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_real &gt;</type>
      <name>u</name>
      <anchorfile>namespaceoperators__test.html</anchorfile>
      <anchor>a694d86dbaf57f62a716d96eb8ce87bd5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_real &gt;</type>
      <name>v</name>
      <anchorfile>namespaceoperators__test.html</anchorfile>
      <anchor>a860ca9fd403e2b071edd997207df63ef</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_complex &gt;</type>
      <name>direct_input</name>
      <anchorfile>namespaceoperators__test.html</anchorfile>
      <anchor>aaad01d24d1b8fb40525dcc0504d71408</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_complex &gt;</type>
      <name>expected_direct</name>
      <anchorfile>namespaceoperators__test.html</anchorfile>
      <anchor>a8adeb5e5a9b6c5533348aadfe8e56bde</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_complex &gt;</type>
      <name>indirect_input</name>
      <anchorfile>namespaceoperators__test.html</anchorfile>
      <anchor>a662ce3a3cab05aa16f244d683370e790</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_complex &gt;</type>
      <name>expected_indirect</name>
      <anchorfile>namespaceoperators__test.html</anchorfile>
      <anchor>a40745badd4fc499fa6c184d038e353fd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_complex &gt;</type>
      <name>expected_S</name>
      <anchorfile>namespaceoperators__test.html</anchorfile>
      <anchor>a8fc882fbaad8516caa74583448397b2d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>operators.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/operators_8h</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="kernels_8h" name="kernels.h" local="yes" imported="no">purify/kernels.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::details</namespace>
    <namespace>purify::operators</namespace>
    <namespace>purify::measurementoperator</namespace>
    <member kind="enumeration">
      <type></type>
      <name>fftw_plan</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</anchorfile>
      <anchor>a39f06b7cbec98e0ab8c7d05f643d5db0</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html" anchor="a39f06b7cbec98e0ab8c7d05f643d5db0a94e7e210543475d2dc1ee610e7b4af1d">estimate</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html" anchor="a39f06b7cbec98e0ab8c7d05f643d5db0a2925f77709f2db1f915deadc2940db88">measure</enumvalue>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>init_gridding_matrix_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1details.html</anchorfile>
      <anchor>a96036c7a586e56d378dbbb4dad742144</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;u, const Vector&lt; t_real &gt; &amp;v, const Vector&lt; t_complex &gt; &amp;weights, const t_uint &amp;imsizey_, const t_uint &amp;imsizex_, const t_real &amp;oversample_ratio, const std::function&lt; t_real(t_real)&gt; kernelu, const std::function&lt; t_real(t_real)&gt; kernelv, const t_uint Ju, const t_uint Jv)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>init_gridding_matrix_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1details.html</anchorfile>
      <anchor>aadad7ec0a0d88c5ddd10f28a27d1b4a5</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;u, const Vector&lt; t_real &gt; &amp;v, const Vector&lt; t_real &gt; &amp;w, const Vector&lt; t_complex &gt; &amp;weights, const t_uint imsizey_, const t_uint imsizex_, const t_real oversample_ratio, const std::function&lt; t_real(t_real)&gt; &amp;ftkerneluv, const std::function&lt; t_real(t_real)&gt; &amp;kerneluv, const t_uint Ju, const t_uint Jw, const t_real cellx, const t_real celly, const t_real abs_error, const t_real rel_error, const dde_type dde)</arglist>
    </member>
    <member kind="function">
      <type>Image&lt; t_complex &gt;</type>
      <name>init_correction2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1details.html</anchorfile>
      <anchor>a288d7453cd72dededadfb96cfcad60b4</anchor>
      <arglist>(const t_real &amp;oversample_ratio, const t_uint &amp;imsizey_, const t_uint &amp;imsizex_, const std::function&lt; t_real(t_real)&gt; ftkernelu, const std::function&lt; t_real(t_real)&gt; ftkernelv, const t_real &amp;w_mean, const t_real &amp;cellx, const t_real &amp;celly)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>init_gridding_matrix_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1details.html</anchorfile>
      <anchor>a196bc201d3a368eeeadafc2ca859368a</anchor>
      <arglist>(const Sparse&lt; T &gt; &amp;mixing_matrix, ARGS &amp;&amp;... args)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; sopt::OperatorFunction&lt; T &gt;, sopt::OperatorFunction&lt; T &gt; &gt;</type>
      <name>init_gridding_matrix_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</anchorfile>
      <anchor>ac94ce709c2bbcf2f07d31a68fe962f82</anchor>
      <arglist>(ARGS &amp;&amp;... args)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; sopt::OperatorFunction&lt; T &gt;, sopt::OperatorFunction&lt; T &gt; &gt;</type>
      <name>init_zero_padding_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</anchorfile>
      <anchor>a1ece84ba389010359b10f07ee76bc984</anchor>
      <arglist>(const Image&lt; typename T::Scalar &gt; &amp;S, const t_real &amp;oversample_ratio)</arglist>
    </member>
    <member kind="function">
      <type>sopt::OperatorFunction&lt; T &gt;</type>
      <name>init_normalise</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</anchorfile>
      <anchor>acbb0f25b649b725caa29142a3c9acb43</anchor>
      <arglist>(const t_real &amp;op_norm)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; sopt::OperatorFunction&lt; T &gt;, sopt::OperatorFunction&lt; T &gt; &gt;</type>
      <name>init_weights_</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</anchorfile>
      <anchor>a1dc82eed77f88b50dd4c2689c88d53fe</anchor>
      <arglist>(const Vector&lt; t_complex &gt; &amp;weights)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; sopt::OperatorFunction&lt; T &gt;, sopt::OperatorFunction&lt; T &gt; &gt;</type>
      <name>init_FFT_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</anchorfile>
      <anchor>adb16231e49aafda70e2b117b4d6190fc</anchor>
      <arglist>(const t_uint &amp;imsizey_, const t_uint &amp;imsizex_, const t_real &amp;oversample_factor_, const fftw_plan fftw_plan_flag_=fftw_plan::measure)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; sopt::OperatorFunction&lt; T &gt;, sopt::OperatorFunction&lt; T &gt; &gt;</type>
      <name>base_padding_and_FFT_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</anchorfile>
      <anchor>ab5cfa89a68b63860b961d36535048b4b</anchor>
      <arglist>(const std::function&lt; t_real(t_real)&gt; &amp;ftkernelu, const std::function&lt; t_real(t_real)&gt; &amp;ftkernelv, const t_uint &amp;imsizey, const t_uint &amp;imsizex, const t_real &amp;oversample_ratio=2, const fftw_plan &amp;ft_plan=fftw_plan::measure, const t_real &amp;w_mean=0, const t_real &amp;cellx=1, const t_real &amp;celly=1)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; sopt::OperatorFunction&lt; T &gt;, sopt::OperatorFunction&lt; T &gt; &gt;</type>
      <name>base_degrid_operator_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</anchorfile>
      <anchor>a74b5c5a09a17518b70fb103a89444740</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;u, const Vector&lt; t_real &gt; &amp;v, const Vector&lt; t_real &gt; &amp;w, const Vector&lt; t_complex &gt; &amp;weights, const t_uint &amp;imsizey, const t_uint &amp;imsizex, const t_real &amp;oversample_ratio=2, const kernels::kernel kernel=kernels::kernel::kb, const t_uint Ju=4, const t_uint Jv=4, const fftw_plan &amp;ft_plan=fftw_plan::measure, const bool w_stacking=false, const t_real &amp;cellx=1, const t_real &amp;celly=1)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; sopt::LinearTransform&lt; T &gt; &gt;</type>
      <name>init_degrid_operator_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1measurementoperator.html</anchorfile>
      <anchor>aff5a8794ed28522484f6b2fdae3724cb</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;u, const Vector&lt; t_real &gt; &amp;v, const Vector&lt; t_real &gt; &amp;w, const Vector&lt; t_complex &gt; &amp;weights, const t_uint &amp;imsizey, const t_uint &amp;imsizex, const t_real &amp;oversample_ratio=2, const kernels::kernel kernel=kernels::kernel::kb, const t_uint Ju=4, const t_uint Jv=4, const bool w_stacking=false, const t_real &amp;cellx=1, const t_real &amp;celly=1)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; sopt::LinearTransform&lt; T &gt; &gt;</type>
      <name>init_degrid_operator_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1measurementoperator.html</anchorfile>
      <anchor>aa97759ff26b02066f150a1f83f447db6</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis_input, const t_uint &amp;imsizey, const t_uint &amp;imsizex, const t_real &amp;cell_x, const t_real &amp;cell_y, const t_real &amp;oversample_ratio=2, const kernels::kernel kernel=kernels::kernel::kb, const t_uint Ju=4, const t_uint Jv=4, const bool w_stacking=false)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>operators_gpu.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/operators__gpu_8h</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
  </compound>
  <compound kind="file">
    <name>pfitsio.cc</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/pfitsio_8cc</filename>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::pfitsio</namespace>
    <member kind="function">
      <type>void</type>
      <name>write_key</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a06beccf538c8dfa4ecc7826ed03ac6ff</anchor>
      <arglist>(fitsfile *fptr, const std::string &amp;key, const std::string &amp;value, const std::string &amp;comment, int *status)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write_key</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a0866652eb43862a9e7354b25d062810f</anchor>
      <arglist>(fitsfile *fptr, const std::string &amp;key, const char *value, const std::string &amp;comment, int *status)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write_history</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a8dd66c7d9d4daaa55b8d022606c1af65</anchor>
      <arglist>(fitsfile *fptr, const std::string &amp;context, const std::string &amp;history, int *status)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>read_key</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a431c4446cdb4d1b0ea4c2ebd6f4458fc</anchor>
      <arglist>(fitsfile *fptr, const std::string &amp;key, int *status)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a05de3285d7ec5d5a6e43d2e5761e8b12</anchor>
      <arglist>(const Image&lt; t_real &gt; &amp;eigen_image, const pfitsio::header_params &amp;header, const bool &amp;overwrite)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>aeeb2e8fe8e4e388c9846e497b4cb8382</anchor>
      <arglist>(const Image&lt; t_real &gt; &amp;image, const std::string &amp;fits_name, const std::string &amp;pix_units=&quot;Jy/Beam&quot;, const bool &amp;overwrite=true)</arglist>
    </member>
    <member kind="function">
      <type>Image&lt; t_complex &gt;</type>
      <name>read2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a2d7e29052f62225e3af9b8e8f1420fb4</anchor>
      <arglist>(const std::string &amp;fits_name)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write3d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a5cc9ae3bb5b792cc11b4be74088a7754</anchor>
      <arglist>(const std::vector&lt; Image&lt; t_real &gt;&gt; &amp;image, const pfitsio::header_params &amp;header, const bool &amp;overwrite=true)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write3d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>ab2064cf1926a00a2aeeeece32959c465</anchor>
      <arglist>(const std::vector&lt; Image&lt; t_real &gt;&gt; &amp;image, const std::string &amp;fits_name, const std::string &amp;pix_units=&quot;Jy/Beam&quot;, const bool &amp;overwrite=true)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Image&lt; t_complex &gt; &gt;</type>
      <name>read3d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a6899ef51fc550bbb6a8188f2f305b889</anchor>
      <arglist>(const std::string &amp;fits_name)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>pfitsio.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/pfitsio_8h</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <class kind="struct">purify::pfitsio::header_params</class>
    <namespace>purify</namespace>
    <namespace>purify::pfitsio</namespace>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_MACRO</name>
      <anchorfile>pfitsio_8h.html</anchorfile>
      <anchor>ae5e7423e6f677807cacaaf36ad9141b3</anchor>
      <arglist>(VAR, H2)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PURIFY_MACRO</name>
      <anchorfile>pfitsio_8h.html</anchorfile>
      <anchor>a079b185e77c44277d7a699604aee2119</anchor>
      <arglist>(KEY, VALUE, COMMENT)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write_key</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a06beccf538c8dfa4ecc7826ed03ac6ff</anchor>
      <arglist>(fitsfile *fptr, const std::string &amp;key, const std::string &amp;value, const std::string &amp;comment, int *status)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write_key</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a0866652eb43862a9e7354b25d062810f</anchor>
      <arglist>(fitsfile *fptr, const std::string &amp;key, const char *value, const std::string &amp;comment, int *status)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write_history</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a8dd66c7d9d4daaa55b8d022606c1af65</anchor>
      <arglist>(fitsfile *fptr, const std::string &amp;context, const std::string &amp;history, int *status)</arglist>
    </member>
    <member kind="function">
      <type>std::enable_if&lt; std::is_scalar&lt; T &gt;::value, void &gt;::type</type>
      <name>write_history</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a585686ddb75d2e64e0c86c4efec3fdab</anchor>
      <arglist>(fitsfile *fptr, const std::string &amp;context, const T &amp;history, int *status)</arglist>
    </member>
    <member kind="function">
      <type>std::enable_if&lt; std::is_scalar&lt; T &gt;::value, void &gt;::type</type>
      <name>write_key</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a0a86095fdf622ce509cee700e48dba2e</anchor>
      <arglist>(fitsfile *fptr, const std::string &amp;key, const T &amp;value, const std::string &amp;comment, int *status)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>read_key</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a431c4446cdb4d1b0ea4c2ebd6f4458fc</anchor>
      <arglist>(fitsfile *fptr, const std::string &amp;key, int *status)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a05de3285d7ec5d5a6e43d2e5761e8b12</anchor>
      <arglist>(const Image&lt; t_real &gt; &amp;eigen_image, const pfitsio::header_params &amp;header, const bool &amp;overwrite)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>aeeb2e8fe8e4e388c9846e497b4cb8382</anchor>
      <arglist>(const Image&lt; t_real &gt; &amp;image, const std::string &amp;fits_name, const std::string &amp;pix_units=&quot;Jy/Beam&quot;, const bool &amp;overwrite=true)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a3e8e2c90dd18fd3343091b6e83213f60</anchor>
      <arglist>(const Eigen::EigenBase&lt; DERIVED &gt; &amp;input, int nx, int ny, const std::string &amp;fits_name, const std::string &amp;pix_units=&quot;Jy/Beam&quot;, const bool &amp;overwrite=true)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>abc28fee9e31953d95f3ddf065539a58c</anchor>
      <arglist>(const Eigen::EigenBase&lt; DERIVED &gt; &amp;input, int nx, int ny, const pfitsio::header_params &amp;header, const bool overwrite=true)</arglist>
    </member>
    <member kind="function">
      <type>Image&lt; t_complex &gt;</type>
      <name>read2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a2d7e29052f62225e3af9b8e8f1420fb4</anchor>
      <arglist>(const std::string &amp;fits_name)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write3d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a5cc9ae3bb5b792cc11b4be74088a7754</anchor>
      <arglist>(const std::vector&lt; Image&lt; t_real &gt;&gt; &amp;image, const pfitsio::header_params &amp;header, const bool &amp;overwrite=true)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write3d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>ab2064cf1926a00a2aeeeece32959c465</anchor>
      <arglist>(const std::vector&lt; Image&lt; t_real &gt;&gt; &amp;image, const std::string &amp;fits_name, const std::string &amp;pix_units=&quot;Jy/Beam&quot;, const bool &amp;overwrite=true)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write3d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a3aa5fe65315f8b330c7111d9d48f1cca</anchor>
      <arglist>(const std::vector&lt; Eigen::EigenBase&lt; DERIVED &gt;&gt; &amp;input, int nx, int ny, const std::string &amp;fits_name, const std::string &amp;pix_units=&quot;Jy/Beam&quot;, const bool &amp;overwrite=true)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write3d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>af097e381fa7d660f76098f5666274e90</anchor>
      <arglist>(const std::vector&lt; Eigen::EigenBase&lt; DERIVED &gt;&gt; &amp;input, int nx, int ny, const pfitsio::header_params &amp;header, const bool &amp;overwrite=true)</arglist>
    </member>
    <member kind="function">
      <type>std::enable_if&lt; std::is_same&lt; t_real, typename T::Scalar &gt;::value, void &gt;::type</type>
      <name>write3d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a8850eb26ccec2d0956f8e27b11a15a04</anchor>
      <arglist>(const Eigen::EigenBase&lt; T &gt; &amp;image, const t_int rows, const t_int cols, const t_int chans, const std::string &amp;fits_name, const std::string &amp;pix_units=&quot;Jy/Beam&quot;, const bool &amp;overwrite=true)</arglist>
    </member>
    <member kind="function">
      <type>std::enable_if&lt; std::is_same&lt; t_real, typename T::Scalar &gt;::value, void &gt;::type</type>
      <name>write3d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>ac97fabc4b92258d4c306cfd486c253c0</anchor>
      <arglist>(const Eigen::EigenBase&lt; T &gt; &amp;image, const t_int rows, const t_int cols, const t_int chans, const pfitsio::header_params &amp;header, const bool &amp;overwrite)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Image&lt; t_complex &gt; &gt;</type>
      <name>read3d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a6899ef51fc550bbb6a8188f2f305b889</anchor>
      <arglist>(const std::string &amp;fits_name)</arglist>
    </member>
    <member kind="function">
      <type>std::enable_if&lt; std::is_same&lt; t_real, typename T::Scalar &gt;::value, void &gt;::type</type>
      <name>read3d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a252309ca86301bfadce2323718d01178</anchor>
      <arglist>(const std::string &amp;fits_name, Eigen::EigenBase&lt; T &gt; &amp;output, t_int &amp;rows, t_int &amp;cols, t_int &amp;channels, t_int &amp;pols)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>read_measurements.cc</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/purify_2read__measurements_8cc</filename>
    <includes id="read__measurements_8h" name="read_measurements.h" local="yes" imported="no">purify/read_measurements.h</includes>
    <includes id="uvfits_8h" name="uvfits.h" local="yes" imported="no">purify/uvfits.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::read_measurements</namespace>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_measurements</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html</anchorfile>
      <anchor>a84cbb7fbe2a5562c37ed918ab4f57f92</anchor>
      <arglist>(const std::string &amp;name, const bool w_term=false, const stokes pol=stokes::I, const utilities::vis_units units=utilities::vis_units::lambda)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_measurements</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html</anchorfile>
      <anchor>a27cfe930b0c597cc75a41b1dc0c4323c</anchor>
      <arglist>(const std::vector&lt; std::string &gt; &amp;names, const bool w_term=false, const stokes pol=stokes::I, const utilities::vis_units units=utilities::vis_units::lambda)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>file_exists</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html</anchorfile>
      <anchor>a58ca8b3a5ff4855f72be8729145d3f54</anchor>
      <arglist>(const std::string &amp;path)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dir_exists</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html</anchorfile>
      <anchor>a433571b90f8e372071cafeda9632180f</anchor>
      <arglist>(const std::string &amp;path)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::string &gt;</type>
      <name>split</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a75b4261fcc6cbc6e4733d6af03e294e0</anchor>
      <arglist>(const std::string &amp;s, char delim)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mkdir_recursive</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a105e14bc03a66d05ce538e8fce85d78b</anchor>
      <arglist>(const std::string &amp;path)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>read_measurements.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/tests_2read__measurements_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="read__measurements_8h" name="read_measurements.h" local="yes" imported="no">purify/read_measurements.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2read__measurements_8cc.html</anchorfile>
      <anchor>acdf3043499c6978365419002274a7bf2</anchor>
      <arglist>(&quot;mkdir&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2read__measurements_8cc.html</anchorfile>
      <anchor>afd7df3df7aae562f92801f2493f01cdb</anchor>
      <arglist>(&quot;uvfits&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>read_measurements.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/read__measurements_8h</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="distribute_8h" name="distribute.h" local="yes" imported="no">purify/distribute.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::read_measurements</namespace>
    <member kind="enumeration">
      <type></type>
      <name>format</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html</anchorfile>
      <anchor>acd8f0c1e37901dde05b3ca586809723c</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html" anchor="acd8f0c1e37901dde05b3ca586809723cac3e888ce42796ce1d5780a4ea8197f0a">vis</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html" anchor="acd8f0c1e37901dde05b3ca586809723cabb130d30d26131335c78557f26dafbcf">uvfits</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html" anchor="acd8f0c1e37901dde05b3ca586809723caee33e909372d935d190f4fcb2a92d542">ms</enumvalue>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_measurements</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html</anchorfile>
      <anchor>a84cbb7fbe2a5562c37ed918ab4f57f92</anchor>
      <arglist>(const std::string &amp;name, const bool w_term=false, const stokes pol=stokes::I, const utilities::vis_units units=utilities::vis_units::lambda)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_measurements</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html</anchorfile>
      <anchor>a27cfe930b0c597cc75a41b1dc0c4323c</anchor>
      <arglist>(const std::vector&lt; std::string &gt; &amp;names, const bool w_term=false, const stokes pol=stokes::I, const utilities::vis_units units=utilities::vis_units::lambda)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>file_exists</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html</anchorfile>
      <anchor>a58ca8b3a5ff4855f72be8729145d3f54</anchor>
      <arglist>(const std::string &amp;path)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dir_exists</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html</anchorfile>
      <anchor>a433571b90f8e372071cafeda9632180f</anchor>
      <arglist>(const std::string &amp;path)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mkdir_recursive</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a105e14bc03a66d05ce538e8fce85d78b</anchor>
      <arglist>(const std::string &amp;path)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mkdir_recursive</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>aa7644e567d476e9f6123218bd3e0088f</anchor>
      <arglist>(const T &amp;path)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>split</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a0a3cf9b99373268388f8c5dee3255da7</anchor>
      <arglist>(const std::string &amp;s, char delim, T result)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::string &gt;</type>
      <name>split</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a75b4261fcc6cbc6e4733d6af03e294e0</anchor>
      <arglist>(const std::string &amp;s, char delim)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>types.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/types_8h</filename>
    <namespace>purify</namespace>
    <namespace>purify::wproj_utilities</namespace>
    <namespace>purify::wproj_utilities::expansions</namespace>
    <namespace>purify::constant</namespace>
    <member kind="typedef">
      <type>std::complex&lt; float &gt;</type>
      <name>t_complexf</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a426969d60456233432af96386b69e341</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>Eigen::Triplet&lt; t_complex &gt;</type>
      <name>t_tripletList</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a854f899505b10ffd6c555674141c65cd</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>Eigen::SparseMatrix&lt; T, Eigen::RowMajor &gt;</type>
      <name>Sparse</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a9c5d741f10029a85f2a8e51fda9a4e07</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>Eigen::SparseVector&lt; T &gt;</type>
      <name>SparseVector</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a75c895e7cb33354e6559085c4cf5571d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>stokes</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a97d23bf901331999e4ef365e403ba8b8</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8add7536794b63bf90eccfd37f9b147d7f">I</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8af09564c9ca56850d4cd6b3319e541aee">Q</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8a4c614360da93c0a041b22e537de151eb">U</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8a5206560a306a2e085a437fd258eb57ce">V</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8ac51b57a703ba1c5869228690c93e1701">XX</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8a0867f43e27585e019c13f7f4b7c4ab6b">YY</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8a74c53bcd3dcb2bb79993b2fec37d362a">XY</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8a923db922542fbe09e7ff87bb31b2f310">YX</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8a67824ecf84f5816f07b74fa956bdbcd2">LL</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8acb95449a94688af33f6e9bb090cf2936">RR</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8a90a7c45eaffbd575ca6fb361e6d170a4">LR</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8a7f49bbe2f0af1edb6c6cee353d3e204b">RL</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8a44c29edb103a2872f519ad0c9a0fdaaa">P</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>dde_type</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>ad1900deb6bebc3b75891904d6f576df5</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="ad1900deb6bebc3b75891904d6f576df5a15a19970dec9bb028b96daf2bb89af90">wkernel_radial</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="ad1900deb6bebc3b75891904d6f576df5aa06fc4a5c57b80607e8a5d5a577bc796">wkernel_2d</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>series</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities_1_1expansions.html</anchorfile>
      <anchor>a92334b90f8de8f723450b3aca72d9e74</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities_1_1expansions.html" anchor="a92334b90f8de8f723450b3aca72d9e74a334c4a4c42fdb79d7ebc3e73b517e6f8">none</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities_1_1expansions.html" anchor="a92334b90f8de8f723450b3aca72d9e74a7d8bc5f1a8d3787d06ef11c97d4655df">taylor</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities_1_1expansions.html" anchor="a92334b90f8de8f723450b3aca72d9e74a172e4ecb02a864e1e4aa51dcce9d8a47">chebyshev</enumvalue>
    </member>
    <member kind="variable">
      <type>const std::map&lt; stokes, t_int &gt;</type>
      <name>stokes_int</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a28dc6ce59ab78004234888df0d8ba64f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::map&lt; std::string, stokes &gt;</type>
      <name>stokes_string</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a563072861363d38f71744c9e817705c3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const t_real</type>
      <name>pi</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1constant.html</anchorfile>
      <anchor>aa35bce2e736d0453c6005cbc06f1dbe7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const t_real</type>
      <name>c</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1constant.html</anchorfile>
      <anchor>a289e794ad6812bcd3de2fdcc68dfb01b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>update_factory.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/update__factory_8h</filename>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="algorithm__factory_8h" name="algorithm_factory.h" local="yes" imported="no">purify/algorithm_factory.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="cimg_8h" name="cimg.h" local="yes" imported="no">purify/cimg.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::factory</namespace>
    <member kind="function">
      <type>void</type>
      <name>add_updater</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>af6b310e472e594a788004536bb28d09b</anchor>
      <arglist>(std::weak_ptr&lt; Algo &gt; const algo_weak, const t_real step_size_scale, const t_real update_tol, const t_uint update_iters, const pfitsio::header_params &amp;update_solution_header, const pfitsio::header_params &amp;update_residual_header, const t_uint imsizey, const t_uint imsizex, const t_uint sara_size, const bool using_mpi)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>uvfits.cc</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/purify_2uvfits_8cc</filename>
    <includes id="uvfits_8h" name="uvfits.h" local="yes" imported="no">purify/uvfits.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::pfitsio</namespace>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_uvfits</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>ab302818131b95815c67a68477508881d</anchor>
      <arglist>(const std::vector&lt; std::string &gt; &amp;names, const bool flag=true, const stokes pol=stokes::I)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_uvfits</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a0e659096cde08d8f8f2d5f49c9f6e99e</anchor>
      <arglist>(const std::string &amp;vis_name2, const utilities::vis_params &amp;u1, const bool flag=true, const stokes pol=stokes::I)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_uvfits</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a4c0c4a69590d4d6e31eb99261dab097f</anchor>
      <arglist>(const std::string &amp;filename, const bool flag=true, const stokes pol=stokes::I)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>filter_and_combine</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a0f3c5af553b929cdfb8547316063a326</anchor>
      <arglist>(const utilities::vis_params &amp;input, const utilities::vis_params &amp;input2, const Vector&lt; t_complex &gt; stokes_transform, const std::function&lt; bool(t_real, t_real, t_real, t_complex, t_complex, t_real, t_real, t_real, t_complex, t_complex)&gt; &amp;filter=[](const t_real, const t_real, const t_real, const t_complex vis1, const t_complex weight1, const t_real, const t_real, const t_real, const t_complex vis2, const t_complex weight2) { return(weight1.real() &gt; 0.) and(weight2.real() &gt; 0.) and(std::abs(vis1) &gt; 1e-20) and(std::abs(vis2) &gt; 1e-20) and(!std::isnan(vis1.real()) and !std::isnan(vis1.imag())) and(!std::isnan(vis2.real()) and !std::isnan(vis2.imag()));})</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_polarisation</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>ae96a909089a418869696283dd65731bc</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;data, const Matrix&lt; t_real &gt; &amp;coords, const Vector&lt; t_real &gt; &amp;frequencies, const t_uint pol_index1, const t_uint pols, const t_uint baselines, const t_uint channels)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_real &gt;</type>
      <name>read_uvfits_freq</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a2bb1a445fbcf5e31447e1c45e563c3ba</anchor>
      <arglist>(fitsfile *fptr, t_int *status, const t_int &amp;col)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>read_uvfits_freq</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>ae2722e4945a6e8d0e138508eac2c97c4</anchor>
      <arglist>(fitsfile *fptr, t_int *status, Vector&lt; t_real &gt; &amp;output, const t_int &amp;col)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_real &gt;</type>
      <name>read_uvfits_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>ad4ac6ff4de411bcc95d5371b6c7e57b3</anchor>
      <arglist>(fitsfile *fptr, t_int *status, const std::vector&lt; t_int &gt; &amp;naxis, const t_int &amp;baselines)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>read_uvfits_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>abcf70ac31a8fd099e7934bbe3e25302f</anchor>
      <arglist>(fitsfile *fptr, t_int *status, const std::vector&lt; t_int &gt; &amp;naxis, const t_int &amp;baselines, Vector&lt; t_real &gt; &amp;output)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_real &gt;</type>
      <name>read_uvfits_coords</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>ab379a780acc3377f4d94e34535edde48</anchor>
      <arglist>(fitsfile *fptr, t_int *status, const t_int &amp;groups, const t_int &amp;pcount)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>read_value_from_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a4ad9e69e80d82aaf5925b6da550e1d85</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;data, const t_uint col, const t_uint pol, const t_uint pols, const t_uint chan, const t_uint chans, const t_uint baseline, const t_uint baselines)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>read_vis_from_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>af7c72608a0a44fc31e640b6e77be7748</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;data, const t_uint pol, const t_uint pols, const t_uint chan, const t_uint chans, const t_uint baseline, const t_uint baselines)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>read_weight_from_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a5e34256158689b24229b60876e06d174</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;data, const t_uint pol, const t_uint pols, const t_uint chan, const t_uint chans, const t_uint baseline, const t_uint baselines)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>read_uvfits_coords</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a2aed6f63e56ae45b4fe2be1031b59d62</anchor>
      <arglist>(fitsfile *fptr, t_int *status, const t_int &amp;pcount, const t_int &amp;groups, Matrix&lt; t_real &gt; &amp;output)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>read_fits_keys</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a3a4c55cd64e7b4e8650e400a82b8fc40</anchor>
      <arglist>(fitsfile *fptr, t_int *status)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>uvfits.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/tests_2uvfits_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="uvfits_8h" name="uvfits.h" local="yes" imported="no">purify/uvfits.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2uvfits_8cc.html</anchorfile>
      <anchor>a32dee56a106751be36f67f244d4a2304</anchor>
      <arglist>(&quot;readfile&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>uvfits.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/uvfits_8h</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::pfitsio</namespace>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_uvfits</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a4c0c4a69590d4d6e31eb99261dab097f</anchor>
      <arglist>(const std::string &amp;filename, const bool flag=true, const stokes pol=stokes::I)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_uvfits</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>ab302818131b95815c67a68477508881d</anchor>
      <arglist>(const std::vector&lt; std::string &gt; &amp;names, const bool flag=true, const stokes pol=stokes::I)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_uvfits</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a0e659096cde08d8f8f2d5f49c9f6e99e</anchor>
      <arglist>(const std::string &amp;vis_name2, const utilities::vis_params &amp;u1, const bool flag=true, const stokes pol=stokes::I)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>filter_and_combine</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a0f3c5af553b929cdfb8547316063a326</anchor>
      <arglist>(const utilities::vis_params &amp;input, const utilities::vis_params &amp;input2, const Vector&lt; t_complex &gt; stokes_transform, const std::function&lt; bool(t_real, t_real, t_real, t_complex, t_complex, t_real, t_real, t_real, t_complex, t_complex)&gt; &amp;filter=[](const t_real, const t_real, const t_real, const t_complex vis1, const t_complex weight1, const t_real, const t_real, const t_real, const t_complex vis2, const t_complex weight2) { return(weight1.real() &gt; 0.) and(weight2.real() &gt; 0.) and(std::abs(vis1) &gt; 1e-20) and(std::abs(vis2) &gt; 1e-20) and(!std::isnan(vis1.real()) and !std::isnan(vis1.imag())) and(!std::isnan(vis2.real()) and !std::isnan(vis2.imag()));})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>read_fits_keys</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a3a4c55cd64e7b4e8650e400a82b8fc40</anchor>
      <arglist>(fitsfile *fptr, t_int *status)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_real &gt;</type>
      <name>read_uvfits_freq</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a2bb1a445fbcf5e31447e1c45e563c3ba</anchor>
      <arglist>(fitsfile *fptr, t_int *status, const t_int &amp;col)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>read_uvfits_freq</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>ae2722e4945a6e8d0e138508eac2c97c4</anchor>
      <arglist>(fitsfile *fptr, t_int *status, Vector&lt; t_real &gt; &amp;output, const t_int &amp;col)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_real &gt;</type>
      <name>read_uvfits_coords</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>ab379a780acc3377f4d94e34535edde48</anchor>
      <arglist>(fitsfile *fptr, t_int *status, const t_int &amp;groups, const t_int &amp;pcount)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>read_uvfits_coords</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a2aed6f63e56ae45b4fe2be1031b59d62</anchor>
      <arglist>(fitsfile *fptr, t_int *status, const t_int &amp;pcount, const t_int &amp;groups, Matrix&lt; t_real &gt; &amp;output)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_polarisation</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>ae96a909089a418869696283dd65731bc</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;data, const Matrix&lt; t_real &gt; &amp;coords, const Vector&lt; t_real &gt; &amp;frequencies, const t_uint pol_index1, const t_uint pols, const t_uint baselines, const t_uint channels)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_real &gt;</type>
      <name>read_uvfits_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>ad4ac6ff4de411bcc95d5371b6c7e57b3</anchor>
      <arglist>(fitsfile *fptr, t_int *status, const std::vector&lt; t_int &gt; &amp;naxis, const t_int &amp;baselines)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>read_uvfits_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>abcf70ac31a8fd099e7934bbe3e25302f</anchor>
      <arglist>(fitsfile *fptr, t_int *status, const std::vector&lt; t_int &gt; &amp;naxis, const t_int &amp;baselines, Vector&lt; t_real &gt; &amp;output)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>read_value_from_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a4ad9e69e80d82aaf5925b6da550e1d85</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;data, const t_uint col, const t_uint pol, const t_uint pols, const t_uint chan, const t_uint chans, const t_uint baseline, const t_uint baselines)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>read_vis_from_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>af7c72608a0a44fc31e640b6e77be7748</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;data, const t_uint pol, const t_uint pols, const t_uint chan, const t_uint chans, const t_uint baseline, const t_uint baselines)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>read_weight_from_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a5e34256158689b24229b60876e06d174</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;data, const t_uint pol, const t_uint pols, const t_uint chan, const t_uint chans, const t_uint baseline, const t_uint baselines)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>wavelet_operator_factory.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/wavelet__operator__factory_8h</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::factory</namespace>
    <member kind="enumeration">
      <type></type>
      <name>distributed_wavelet_operator</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>a5bbc6a01d0a2cdd30b656a6bb957221f</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a5bbc6a01d0a2cdd30b656a6bb957221fa74136b24217cb75599440e6e1d807cf7">serial</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a5bbc6a01d0a2cdd30b656a6bb957221fad560be68e475e7680c1ea7f87a40926e">mpi_sara</enumvalue>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; sopt::LinearTransform&lt; T &gt; const  &gt;</type>
      <name>wavelet_operator_factory</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>a2ffd3209dbb03d7390df879572cea326</anchor>
      <arglist>(const distributed_wavelet_operator distribute, const std::vector&lt; std::tuple&lt; std::string, t_uint &gt;&gt; &amp;wavelets, const t_uint imsizey, const t_uint imsizex, t_uint &amp;sara_size)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; sopt::LinearTransform&lt; T &gt; const  &gt;</type>
      <name>wavelet_operator_factory</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>a5c435fa169cac5a8135922b5558f3ae6</anchor>
      <arglist>(const distributed_wavelet_operator distribute, const std::vector&lt; std::tuple&lt; std::string, t_uint &gt;&gt; &amp;wavelets, const t_uint imsizey, const t_uint imsizex)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>wide_field_utilities.cc</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/purify_2wide__field__utilities_8cc</filename>
    <includes id="wide__field__utilities_8h" name="wide_field_utilities.h" local="yes" imported="no">wide_field_utilities.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::widefield</namespace>
    <member kind="function">
      <type>t_int</type>
      <name>w_support</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1widefield.html</anchorfile>
      <anchor>a72665d41e585afb454323ad5b3e2a5cc</anchor>
      <arglist>(const t_real w, const t_real du, const t_int min, const t_int max)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>pixel_to_lambda</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1widefield.html</anchorfile>
      <anchor>ab62e83c2e2504a0e5b0e0d333d311b52</anchor>
      <arglist>(const t_real cell, const t_uint imsize, const t_real oversample_ratio)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>estimate_cell_size</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1widefield.html</anchorfile>
      <anchor>afa5d4920d752c25aa211d50635a87b8d</anchor>
      <arglist>(const t_real max_u, const t_uint imsize, const t_real oversample_ratio)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>fov_cosine</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1widefield.html</anchorfile>
      <anchor>a1ea24a256a7408ced82c19ac8afb2de9</anchor>
      <arglist>(t_real const cell, t_uint const imsize)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>generate_chirp</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1widefield.html</anchorfile>
      <anchor>ad220bdbe5a36b5047dee3cc68f66533e</anchor>
      <arglist>(const t_real w_rate, const t_real cell_x, const t_real cell_y, const t_uint x_size, const t_uint y_size)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>wide_field_utilities.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/tests_2wide__field__utilities_8cc</filename>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="convolution_8h" name="convolution.h" local="yes" imported="no">purify/convolution.h</includes>
    <includes id="kernels_8h" name="kernels.h" local="yes" imported="no">purify/kernels.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <includes id="wide__field__utilities_8h" name="wide_field_utilities.h" local="yes" imported="no">purify/wide_field_utilities.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2wide__field__utilities_8cc.html</anchorfile>
      <anchor>a5211b65822ff50a5c544af1ca81c5d69</anchor>
      <arglist>(&quot;uvw units&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>tests_2wide__field__utilities_8cc.html</anchorfile>
      <anchor>a464e25ece9ca5c279c08db3363f37f0e</anchor>
      <arglist>(&quot;Calcuate DDE Image&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>wide_field_utilities.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/wide__field__utilities_8h</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::widefield</namespace>
    <member kind="function">
      <type>t_int</type>
      <name>w_support</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1widefield.html</anchorfile>
      <anchor>a72665d41e585afb454323ad5b3e2a5cc</anchor>
      <arglist>(const t_real w, const t_real du, const t_int min, const t_int max)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>pixel_to_lambda</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1widefield.html</anchorfile>
      <anchor>ab62e83c2e2504a0e5b0e0d333d311b52</anchor>
      <arglist>(const t_real cell, const t_uint imsize, const t_real oversample_ratio)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>estimate_cell_size</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1widefield.html</anchorfile>
      <anchor>afa5d4920d752c25aa211d50635a87b8d</anchor>
      <arglist>(const t_real max_u, const t_uint imsize, const t_real oversample_ratio)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>fov_cosine</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1widefield.html</anchorfile>
      <anchor>a1ea24a256a7408ced82c19ac8afb2de9</anchor>
      <arglist>(t_real const cell, t_uint const imsize)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>generate_dde</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1widefield.html</anchorfile>
      <anchor>af8c53ba24d42ab813cbf4f03f56f2a7d</anchor>
      <arglist>(const DDE &amp;dde, const t_real cell_x, const t_real cell_y, const t_uint x_size, const t_uint y_size)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>generate_chirp</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1widefield.html</anchorfile>
      <anchor>a7dd1480057051aae3314ab9465821ad4</anchor>
      <arglist>(const DDE &amp;dde, const t_real w_rate, const t_real cell_x, const t_real cell_y, const t_uint x_size, const t_uint y_size)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>generate_chirp</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1widefield.html</anchorfile>
      <anchor>ad220bdbe5a36b5047dee3cc68f66533e</anchor>
      <arglist>(const t_real w_rate, const t_real cell_x, const t_real cell_y, const t_uint x_size, const t_uint y_size)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>wkernel_integration.cc</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/wkernel__integration_8cc</filename>
    <includes id="wkernel__integration_8h" name="wkernel_integration.h" local="yes" imported="no">wkernel_integration.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::projection_kernels</namespace>
    <member kind="function">
      <type>t_complex</type>
      <name>fourier_wproj_kernel</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1projection__kernels.html</anchorfile>
      <anchor>abf118d404395c7699556217d0474950e</anchor>
      <arglist>(const t_real x, const t_real y, const t_real w, const t_real u, const t_real v, const t_real du, const t_real dv)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>hankel_wproj_kernel</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1projection__kernels.html</anchorfile>
      <anchor>aea4249664d112544ddb02235e8f972d2</anchor>
      <arglist>(const t_real r, const t_real w, const t_real u, const t_real v, const t_real du)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>exact_w_projection_integration_1d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1projection__kernels.html</anchorfile>
      <anchor>a1268ba056206a35e13d2faad51da6990</anchor>
      <arglist>(const t_real u, const t_real v, const t_real w, const t_real du, const t_real oversample_ratio, const std::function&lt; t_complex(t_real)&gt; &amp;ftkerneluv, const t_uint &amp;max_evaluations, const t_real &amp;absolute_error, const t_real &amp;relative_error, const integration::method method, t_uint &amp;evaluations)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>exact_w_projection_integration</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1projection__kernels.html</anchorfile>
      <anchor>a3363eb7199ce493e3b828292c7ece53a</anchor>
      <arglist>(const t_real u, const t_real v, const t_real w, const t_real du, const t_real dv, const t_real oversample_ratio, const std::function&lt; t_complex(t_real)&gt; &amp;ftkernelu, const std::function&lt; t_complex(t_real)&gt; &amp;ftkernelv, const t_uint &amp;max_evaluations, const t_real &amp;absolute_error, const t_real &amp;relative_error, const integration::method method, t_uint &amp;evaluations)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>wkernel_integration.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/wkernel__integration_8h</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="integration_8h" name="integration.h" local="yes" imported="no">purify/integration.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::projection_kernels</namespace>
    <member kind="function">
      <type>t_complex</type>
      <name>fourier_wproj_kernel</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1projection__kernels.html</anchorfile>
      <anchor>abf118d404395c7699556217d0474950e</anchor>
      <arglist>(const t_real x, const t_real y, const t_real w, const t_real u, const t_real v, const t_real du, const t_real dv)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>hankel_wproj_kernel</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1projection__kernels.html</anchorfile>
      <anchor>aea4249664d112544ddb02235e8f972d2</anchor>
      <arglist>(const t_real r, const t_real w, const t_real u, const t_real v, const t_real du)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>exact_w_projection_integration_1d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1projection__kernels.html</anchorfile>
      <anchor>a1268ba056206a35e13d2faad51da6990</anchor>
      <arglist>(const t_real u, const t_real v, const t_real w, const t_real du, const t_real oversample_ratio, const std::function&lt; t_complex(t_real)&gt; &amp;ftkerneluv, const t_uint &amp;max_evaluations, const t_real &amp;absolute_error, const t_real &amp;relative_error, const integration::method method, t_uint &amp;evaluations)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>exact_w_projection_integration</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1projection__kernels.html</anchorfile>
      <anchor>a3363eb7199ce493e3b828292c7ece53a</anchor>
      <arglist>(const t_real u, const t_real v, const t_real w, const t_real du, const t_real dv, const t_real oversample_ratio, const std::function&lt; t_complex(t_real)&gt; &amp;ftkernelu, const std::function&lt; t_complex(t_real)&gt; &amp;ftkernelv, const t_uint &amp;max_evaluations, const t_real &amp;absolute_error, const t_real &amp;relative_error, const integration::method method, t_uint &amp;evaluations)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>wproj_operators.cc</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/wproj__operators_8cc</filename>
    <includes id="wproj__operators_8h" name="wproj_operators.h" local="yes" imported="no">purify/wproj_operators.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="wide__field__utilities_8h" name="wide_field_utilities.h" local="yes" imported="no">purify/wide_field_utilities.h</includes>
    <includes id="wkernel__integration_8h" name="wkernel_integration.h" local="yes" imported="no">purify/wkernel_integration.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::details</namespace>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>init_gridding_matrix_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1details.html</anchorfile>
      <anchor>aadad7ec0a0d88c5ddd10f28a27d1b4a5</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;u, const Vector&lt; t_real &gt; &amp;v, const Vector&lt; t_real &gt; &amp;w, const Vector&lt; t_complex &gt; &amp;weights, const t_uint imsizey_, const t_uint imsizex_, const t_real oversample_ratio, const std::function&lt; t_real(t_real)&gt; &amp;ftkerneluv, const std::function&lt; t_real(t_real)&gt; &amp;kerneluv, const t_uint Ju, const t_uint Jw, const t_real cellx, const t_real celly, const t_real abs_error, const t_real rel_error, const dde_type dde)</arglist>
    </member>
    <member kind="function">
      <type>Image&lt; t_complex &gt;</type>
      <name>init_correction_radial_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1details.html</anchorfile>
      <anchor>a8baba6d82e8930a1b2571c541a660669</anchor>
      <arglist>(const t_real oversample_ratio, const t_uint imsizey_, const t_uint imsizex_, const std::function&lt; t_real(t_real)&gt; &amp;ftkerneluv, const t_real w_mean, const t_real cellx, const t_real celly)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>wproj_operators.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/wproj__operators_8h</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::details</namespace>
    <namespace>purify::operators</namespace>
    <namespace>purify::measurementoperator</namespace>
    <member kind="function">
      <type>Image&lt; t_complex &gt;</type>
      <name>init_correction_radial_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1details.html</anchorfile>
      <anchor>a8baba6d82e8930a1b2571c541a660669</anchor>
      <arglist>(const t_real oversample_ratio, const t_uint imsizey_, const t_uint imsizex_, const std::function&lt; t_real(t_real)&gt; &amp;ftkerneluv, const t_real w_mean, const t_real cellx, const t_real celly)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; sopt::OperatorFunction&lt; T &gt;, sopt::OperatorFunction&lt; T &gt; &gt;</type>
      <name>base_padding_and_FFT_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</anchorfile>
      <anchor>a5a9e812998fc6df0645d778f0201b7bd</anchor>
      <arglist>(const std::function&lt; t_real(t_real)&gt; &amp;ftkerneluv, const t_uint imsizey, const t_uint imsizex, const t_real oversample_ratio, const fftw_plan ft_plan, const t_real w_mean, const t_real cellx, const t_real celly)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; sopt::OperatorFunction&lt; T &gt;, sopt::OperatorFunction&lt; T &gt; &gt;</type>
      <name>base_degrid_operator_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</anchorfile>
      <anchor>a921459c522a44106315cf0a0500c43c0</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;u, const Vector&lt; t_real &gt; &amp;v, const Vector&lt; t_real &gt; &amp;w, const Vector&lt; t_complex &gt; &amp;weights, const t_uint &amp;imsizey, const t_uint &amp;imsizex, const t_real oversample_ratio, const kernels::kernel kernel, const t_uint Ju, const t_uint Jw, const fftw_plan ft_plan, const bool w_stacking, const t_real cellx, const t_real celly, const t_real absolute_error, const t_real relative_error, const dde_type dde)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; sopt::LinearTransform&lt; T &gt; &gt;</type>
      <name>init_degrid_operator_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1measurementoperator.html</anchorfile>
      <anchor>af2d03beb248bc90cbce87b61a676993c</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;u, const Vector&lt; t_real &gt; &amp;v, const Vector&lt; t_real &gt; &amp;w, const Vector&lt; t_complex &gt; &amp;weights, const t_uint imsizey, const t_uint imsizex, const t_real oversample_ratio, const kernels::kernel kernel, const t_uint Ju, const t_uint Jw, const bool w_stacking, const t_real cellx, const t_real celly, const t_real absolute_error, const t_real relative_error, const dde_type dde)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; sopt::LinearTransform&lt; T &gt; &gt;</type>
      <name>init_degrid_operator_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1measurementoperator.html</anchorfile>
      <anchor>af14d4bff532ed08e5f31dc24d0e1bde2</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis_input, const t_uint imsizey, const t_uint imsizex, const t_real cell_x, const t_real cell_y, const t_real oversample_ratio, const kernels::kernel kernel, const t_uint Ju, const t_uint Jw, const bool w_stacking, const t_real absolute_error, const t_real relative_error, const dde_type dde)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>wproj_operators_gpu.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/wproj__operators__gpu_8h</filename>
    <includes id="operators__gpu_8h" name="operators_gpu.h" local="yes" imported="no">purify/operators_gpu.h</includes>
    <includes id="wproj__operators_8h" name="wproj_operators.h" local="yes" imported="no">purify/wproj_operators.h</includes>
  </compound>
  <compound kind="file">
    <name>wproj_utilities.cc</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/wproj__utilities_8cc</filename>
    <includes id="wproj__utilities_8h" name="wproj_utilities.h" local="yes" imported="no">purify/wproj_utilities.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::utilities</namespace>
    <namespace>purify::wproj_utilities</namespace>
    <member kind="function">
      <type>vis_params</type>
      <name>sort_by_w</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>af19ddf9b8eb98a1298d163db8472b663</anchor>
      <arglist>(const vis_params &amp;uv_data)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; t_uint &gt;</type>
      <name>w_rows</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a028e2635db8ad772b8c14216349f4de2</anchor>
      <arglist>(const Sparse&lt; t_complex &gt; &amp;w_degrider)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; t_real, t_real &gt;</type>
      <name>fov_cosines</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a55409e6c696934e139f47f0930285538</anchor>
      <arglist>(t_real const &amp;cell_x, t_real const &amp;cell_y, t_uint const &amp;x_size, t_uint const &amp;y_size)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>generate_dde</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a9c8340087c9a825336f8e1d511248d20</anchor>
      <arglist>(const std::function&lt; t_complex(t_real, t_real)&gt; &amp;dde, const t_real &amp;cell_x, const t_real &amp;cell_y, const t_uint &amp;x_size, const t_uint &amp;y_size)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>generate_chirp</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a44e1cf3c127592ab55a696769d40bc1b</anchor>
      <arglist>(const std::function&lt; t_complex(t_real, t_real)&gt; &amp;dde, const t_real &amp;w_rate, const t_real &amp;cell_x, const t_real &amp;cell_y, const t_uint &amp;x_size, const t_uint &amp;y_size)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>generate_chirp</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a2c06254253286f0e080fd31494c9afb3</anchor>
      <arglist>(const t_real &amp;w_rate, const t_real &amp;cell_x, const t_real &amp;cell_y, const t_uint &amp;x_size, const t_uint &amp;y_size)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>create_chirp_row</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a64fb8d0d62d0bccbd328c95e4146f510</anchor>
      <arglist>(const t_real &amp;w_rate, const t_real &amp;cell_x, const t_real &amp;cell_y, const t_uint &amp;ftsizev, const t_uint &amp;ftsizeu, const t_real &amp;energy_fraction, const sopt::OperatorFunction&lt; Vector&lt; t_complex &gt;&gt; &amp;fftop)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>create_chirp_row</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a72764af9467ca2f6ef54561861c71fdb</anchor>
      <arglist>(const Vector&lt; t_complex &gt; &amp;chirp_image, const t_real &amp;energy_fraction, const sopt::OperatorFunction&lt; Vector&lt; t_complex &gt;&gt; &amp;fftop)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>wprojection_matrix</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>af77eb45df90553de1b6d470cc2812d7a</anchor>
      <arglist>(const Sparse&lt; t_complex &gt; &amp;G, const t_uint &amp;x_size, const t_uint &amp;y_size, const Vector&lt; t_real &gt; &amp;w_components, const t_real &amp;cell_x, const t_real &amp;cell_y, const t_real &amp;energy_fraction_chirp, const t_real &amp;energy_fraction_wproj, const expansions::series series=expansions::series::none, const t_uint order=1, const t_real &amp;interpolation_error=1e-2)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>snr_metric</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a17c5b50902a73725b22c031f40bf1cae</anchor>
      <arglist>(const Image&lt; t_real &gt; &amp;model, const Image&lt; t_real &gt; &amp;solution)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>mr_metric</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>ac91e4de3e44bf6b1a10b266f2768ca3e</anchor>
      <arglist>(const Image&lt; t_real &gt; &amp;model, const Image&lt; t_real &gt; &amp;solution)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>generate_vect</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>ab2721935f8875cce834fd3f8d01cfd2e</anchor>
      <arglist>(const t_uint &amp;x_size, const t_uint &amp;y_size, const t_real &amp;sigma, const t_real &amp;mean)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>wproj_utilities.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/wproj__utilities_8h</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::utilities</namespace>
    <namespace>purify::wproj_utilities</namespace>
    <member kind="function">
      <type>vis_params</type>
      <name>sort_by_w</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>af19ddf9b8eb98a1298d163db8472b663</anchor>
      <arglist>(const vis_params &amp;uv_data)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>w_lambert</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a0a3636b5cc986bcc48ed8a437d4c1f06</anchor>
      <arglist>(T x, const t_real &amp;relative_difference)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; t_real, t_real &gt;</type>
      <name>fov_cosines</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a55409e6c696934e139f47f0930285538</anchor>
      <arglist>(t_real const &amp;cell_x, t_real const &amp;cell_y, t_uint const &amp;x_size, t_uint const &amp;y_size)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>convert_sparse</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a7a85d11b4c738e9808ef35ce971174bc</anchor>
      <arglist>(const Eigen::MatrixBase&lt; T &gt; &amp;M, const t_real &amp;threshold=0.)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>generate_dde</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a9c8340087c9a825336f8e1d511248d20</anchor>
      <arglist>(const std::function&lt; t_complex(t_real, t_real)&gt; &amp;dde, const t_real &amp;cell_x, const t_real &amp;cell_y, const t_uint &amp;x_size, const t_uint &amp;y_size)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>generate_chirp</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a2c06254253286f0e080fd31494c9afb3</anchor>
      <arglist>(const t_real &amp;w_rate, const t_real &amp;cell_x, const t_real &amp;cell_y, const t_uint &amp;x_size, const t_uint &amp;y_size)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>generate_chirp</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a44e1cf3c127592ab55a696769d40bc1b</anchor>
      <arglist>(const std::function&lt; t_complex(t_real, t_real)&gt; &amp;dde, const t_real &amp;w_rate, const t_real &amp;cell_x, const t_real &amp;cell_y, const t_uint &amp;x_size, const t_uint &amp;y_size)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>create_chirp_row</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a64fb8d0d62d0bccbd328c95e4146f510</anchor>
      <arglist>(const t_real &amp;w_rate, const t_real &amp;cell_x, const t_real &amp;cell_y, const t_uint &amp;ftsizev, const t_uint &amp;ftsizeu, const t_real &amp;energy_fraction, const sopt::OperatorFunction&lt; Vector&lt; t_complex &gt;&gt; &amp;fftop)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>create_chirp_row</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a72764af9467ca2f6ef54561861c71fdb</anchor>
      <arglist>(const Vector&lt; t_complex &gt; &amp;chirp_image, const t_real &amp;energy_fraction, const sopt::OperatorFunction&lt; Vector&lt; t_complex &gt;&gt; &amp;fftop)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>sparsify_row_thres</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a4d084f0a41724ba4bcf53ffaff6f2968</anchor>
      <arglist>(const Eigen::SparseMatrixBase&lt; T &gt; &amp;row, const t_real &amp;energy)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>sparsify_row_dense_thres</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a5abb6ec3b503e160ad37487c1bfe34fd</anchor>
      <arglist>(const Eigen::MatrixBase&lt; T &gt; &amp;row, const t_real &amp;energy)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>row_wise_convolution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a77c1fcdca84805f84e0c7ae2f1b15746</anchor>
      <arglist>(const Sparse&lt; t_complex &gt; &amp;Grid_, const Sparse&lt; T &gt; &amp;chirp_, const t_uint &amp;x_size, const t_uint &amp;y_size)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>wprojection_matrix</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>af77eb45df90553de1b6d470cc2812d7a</anchor>
      <arglist>(const Sparse&lt; t_complex &gt; &amp;G, const t_uint &amp;x_size, const t_uint &amp;y_size, const Vector&lt; t_real &gt; &amp;w_components, const t_real &amp;cell_x, const t_real &amp;cell_y, const t_real &amp;energy_fraction_chirp, const t_real &amp;energy_fraction_wproj, const expansions::series series=expansions::series::none, const t_uint order=1, const t_real &amp;interpolation_error=1e-2)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>snr_metric</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a17c5b50902a73725b22c031f40bf1cae</anchor>
      <arglist>(const Image&lt; t_real &gt; &amp;model, const Image&lt; t_real &gt; &amp;solution)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>mr_metric</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>ac91e4de3e44bf6b1a10b266f2768ca3e</anchor>
      <arglist>(const Image&lt; t_real &gt; &amp;model, const Image&lt; t_real &gt; &amp;solution)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>sparsity_sp</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a40d840c74c357373271393c1a3e814f6</anchor>
      <arglist>(const Sparse&lt; t_complex &gt; &amp;Gmat)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>sparsity_im</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a9514b3dffe2921b9a6cdbe7f1f6d48e9</anchor>
      <arglist>(const Image&lt; t_complex &gt; &amp;Cmat)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>generate_vect</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>ab2721935f8875cce834fd3f8d01cfd2e</anchor>
      <arglist>(const t_uint &amp;x_size, const t_uint &amp;y_size, const t_real &amp;sigma, const t_real &amp;mean)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>upsample_ratio_sim</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>abd072be7657eca4ffa663c86f7d9af0c</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis, const t_real &amp;L, const t_real &amp;M, const t_int &amp;x_size, const t_int &amp;y_size, const t_int &amp;multipleOf)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>row_wise_sparse_convolution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>ae9b2839aeb90d232c87cc0806491f84a</anchor>
      <arglist>(const Sparse&lt; t_complex &gt; &amp;Grid_, const Sparse&lt; T &gt; &amp;chirp_, const t_uint &amp;x_size, const t_uint &amp;y_size)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>w_lambert</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a561048f21d1a1569b5d0d3123c1d45ed</anchor>
      <arglist>(const T &amp;x, const t_real &amp;relative_difference=1e-8)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>yaml-parser.cc</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/yaml-parser_8cc</filename>
    <includes id="yaml-parser_8h" name="yaml-parser.h" local="yes" imported="no">purify/yaml-parser.h</includes>
    <includes id="read__measurements_8h" name="read_measurements.h" local="yes" imported="no">purify/read_measurements.h</includes>
    <namespace>purify</namespace>
    <member kind="function">
      <type>T</type>
      <name>get_vector</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a05b6872149a85881d90befe9d341da05</anchor>
      <arglist>(const YAML::Node &amp;node_map, const std::initializer_list&lt; const char *&gt; indicies)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>yaml-parser.h</name>
    <path>/mydata/cpp/purify/</path>
    <filename>http://astro-informatics.github.io/purify/yaml-parser_8h</filename>
    <includes id="algorithm__factory_8h" name="algorithm_factory.h" local="yes" imported="no">purify/algorithm_factory.h</includes>
    <class kind="class">purify::YamlParser</class>
    <namespace>purify</namespace>
    <member kind="define">
      <type>#define</type>
      <name>YAML_MACRO</name>
      <anchorfile>yaml-parser_8h.html</anchorfile>
      <anchor>abdc301f15a1438ddc24a38f16232f9d6</anchor>
      <arglist>(TYPE, NAME, VALUE)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>algo_factory.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/algo__factory_8cc</filename>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <includes id="algorithm__factory_8h" name="algorithm_factory.h" local="yes" imported="no">purify/algorithm_factory.h</includes>
    <includes id="measurement__operator__factory_8h" name="measurement_operator_factory.h" local="yes" imported="no">purify/measurement_operator_factory.h</includes>
    <includes id="wavelet__operator__factory_8h" name="wavelet_operator_factory.h" local="yes" imported="no">purify/wavelet_operator_factory.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>algo__factory_8cc.html</anchorfile>
      <anchor>a62b7c5992c6489aec5e179308c6329e9</anchor>
      <arglist>(&quot;padmm_factory&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>algo__factory_8cc.html</anchorfile>
      <anchor>a58690f932b4a4be8baefa0eb1add6635</anchor>
      <arglist>(&quot;fb_factory&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>algo__factory_8cc.html</anchorfile>
      <anchor>aeb77d36e680282a52237c8fc801acd4b</anchor>
      <arglist>(&quot;joint_map_factory&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>common_catch_main.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/common__catch__main_8cc</filename>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>CATCH_CONFIG_RUNNER</name>
      <anchorfile>common__catch__main_8cc.html</anchorfile>
      <anchor>a34b4c3eca7342fbc4cba090d02139902</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>std::unique_ptr&lt; std::mt19937_64 &gt;</type>
      <name>mersenne</name>
      <anchorfile>common__catch__main_8cc.html</anchorfile>
      <anchor>afd7b13def75c5d7feb77ac1ea2df5d0b</anchor>
      <arglist>(new std::mt19937_64(0))</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>common__catch__main_8cc.html</anchorfile>
      <anchor>a3c04138a5bfe5d72780bb7e82a18e627</anchor>
      <arglist>(int argc, char **argv)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>common_mpi_catch_main.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/common__mpi__catch__main_8cc</filename>
    <includes id="logging_8h" name="logging.h" local="no" imported="no">purify/logging.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>CATCH_CONFIG_RUNNER</name>
      <anchorfile>common__mpi__catch__main_8cc.html</anchorfile>
      <anchor>a34b4c3eca7342fbc4cba090d02139902</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>std::unique_ptr&lt; std::mt19937_64 &gt;</type>
      <name>mersenne</name>
      <anchorfile>common__mpi__catch__main_8cc.html</anchorfile>
      <anchor>afd7b13def75c5d7feb77ac1ea2df5d0b</anchor>
      <arglist>(new std::mt19937_64(0))</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>common__mpi__catch__main_8cc.html</anchorfile>
      <anchor>a217dbf8b442f20279ea00b898af96f52</anchor>
      <arglist>(int argc, const char **argv)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>convolution.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/convolution_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="convolution_8h" name="convolution.h" local="yes" imported="no">purify/convolution.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>convolution_8cc.html</anchorfile>
      <anchor>a8667455cd30268b79b4e3fa42459eb8a</anchor>
      <arglist>(&quot;1d_zeropad&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>convolution_8cc.html</anchorfile>
      <anchor>a3252236d85c0634ee1dc4454670c6c3e</anchor>
      <arglist>(&quot;1d_convolution&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>convolution_8cc.html</anchorfile>
      <anchor>afd3fdb7d730399aabc63f887cfe615d8</anchor>
      <arglist>(&quot;2d_convolution&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>convolution_8cc.html</anchorfile>
      <anchor>a8e6045240ae31ad085d06932a31f1869</anchor>
      <arglist>(&quot;2d_convolution_functions&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>data.in.h</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/data_8in_8h</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <namespace>purify</namespace>
    <namespace>purify::notinstalled</namespace>
    <member kind="function">
      <type>std::enable_if&lt; std::is_scalar&lt; T &gt;::value, std::vector&lt; T &gt; &gt;::type</type>
      <name>read_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>af7cc59cd1e20fb709a98750d5acfc541</anchor>
      <arglist>(const std::string &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::enable_if&lt; std::is_same&lt; t_complex, T &gt;::value, std::vector&lt; T &gt; &gt;::type</type>
      <name>read_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>a6de72df1755781a71f9b5d0b26585f73</anchor>
      <arglist>(const std::string &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; t_complex &gt;</type>
      <name>read_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>a9fecf19b08db444e65ca342e7e599e53</anchor>
      <arglist>(const std::vector&lt; t_real &gt; &amp;input)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>directories.in.h</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/directories_8in_8h</filename>
    <namespace>purify</namespace>
    <namespace>purify::notinstalled</namespace>
    <member kind="function">
      <type>std::string</type>
      <name>data_directory</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>a2a9204ef1ce008b33ce31abbe59045bf</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>data_filename</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>a31500447173c424a79ff41f27ce64804</anchor>
      <arglist>(std::string const &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>scratch_directory</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>a39f480c23d66c09cc02801c33ea7dc28</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>scratch_filename</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>ad1628b70a7db77d2fee2aa5b1c79fbc8</anchor>
      <arglist>(std::string const &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>image_filename</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>adcd4a748b6eaf349d7a41cfc85428883</anchor>
      <arglist>(std::string const &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>visibility_filename</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>a0f5b36543cab9c5f594e8c3c8191f272</anchor>
      <arglist>(std::string const &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>vla_filename</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>ac34b6abbc10ba7a195b39343a407b941</anchor>
      <arglist>(std::string const &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>atca_filename</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>adf7cca24a1b2c8df495b8c2aadbc61f8</anchor>
      <arglist>(std::string const &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>gridding_filename</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>ac90b295d30fb2cee7bc89403dedc1615</anchor>
      <arglist>(std::string const &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>degridding_filename</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>a27f867ebed051d1c131c979ea8d5d4cc</anchor>
      <arglist>(std::string const &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>output_directory</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>a6e559eebbb8daf09c4a6a297982e9533</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>output_filename</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>a711465428b3f8363776594d7d2db98be</anchor>
      <arglist>(std::string const &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>ngc3256_ms</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>adec66730b5b17d093151b170b6f954fc</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>distribute_sparse_vector.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/distribute__sparse__vector_8cc</filename>
    <includes id="AllToAllSparseVector_8h" name="AllToAllSparseVector.h" local="no" imported="no">purify/AllToAllSparseVector.h</includes>
    <includes id="DistributeSparseVector_8h" name="DistributeSparseVector.h" local="no" imported="no">purify/DistributeSparseVector.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>distribute__sparse__vector_8cc.html</anchorfile>
      <anchor>a5078653d481f24b62cdb68271b50fa39</anchor>
      <arglist>(&quot;Distribe Sparse Vector&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>distribute__sparse__vector_8cc.html</anchorfile>
      <anchor>a8983cadd72e91c37967bc28f2dc23dbc</anchor>
      <arglist>(&quot;All to one Sparse Vector&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>distribute__sparse__vector_8cc.html</anchorfile>
      <anchor>a429b923b49e1897cbb7c30afb0c717cd</anchor>
      <arglist>(&quot;All to All Sparse Vector&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>index_mapping.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/index__mapping_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="IndexMapping_8h" name="IndexMapping.h" local="yes" imported="no">purify/IndexMapping.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>index__mapping_8cc.html</anchorfile>
      <anchor>a5ecfe1eae6567db81af76102ce0df2d7</anchor>
      <arglist>(&quot;Index mapping operator&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>index__mapping_8cc.html</anchorfile>
      <anchor>aad73c738ff433791ffbba8508a5aaba0</anchor>
      <arglist>(&quot;Non empty outer vectors&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>index__mapping_8cc.html</anchorfile>
      <anchor>a12570e3e32aed020b2303e7e8ccd65d8</anchor>
      <arglist>(&quot;Shrink sparse matrix&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>index__mapping_8cc.html</anchorfile>
      <anchor>a531eabfc1f8c07f37db1dace00d8a486</anchor>
      <arglist>(&quot;Vector-shrinked matrix multiplication&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>kmeans.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/kmeans_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="distribute_8h" name="distribute.h" local="yes" imported="no">purify/distribute.h</includes>
    <includes id="mpi__utilities_8h" name="mpi_utilities.h" local="yes" imported="no">purify/mpi_utilities.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>TEST_MACRO</name>
      <anchorfile>kmeans_8cc.html</anchorfile>
      <anchor>a5187dcd48f43858abc36974af5647340</anchor>
      <arglist>(param)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>kmeans_8cc.html</anchorfile>
      <anchor>a1ca7a856fcbb65b7b8e35aa90ef2b01b</anchor>
      <arglist>(&quot;k-means&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>measurement_factory.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/measurement__factory_8cc</filename>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="measurement__operator__factory_8h" name="measurement_operator_factory.h" local="yes" imported="no">purify/measurement_operator_factory.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>measurement__factory_8cc.html</anchorfile>
      <anchor>a4b4144b317ffdd35b81e65d5d926149b</anchor>
      <arglist>(&quot;Serial vs Distributed Operator&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>measurement__factory_8cc.html</anchorfile>
      <anchor>a6f0552812c9180a64360dbba3608a5a3</anchor>
      <arglist>(&quot;GPU Serial vs Distributed Operator&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>mpi_algo_factory.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/mpi__algo__factory_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="distribute_8h" name="distribute.h" local="yes" imported="no">purify/distribute.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="mpi__utilities_8h" name="mpi_utilities.h" local="yes" imported="no">purify/mpi_utilities.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <includes id="algorithm__factory_8h" name="algorithm_factory.h" local="yes" imported="no">purify/algorithm_factory.h</includes>
    <includes id="measurement__operator__factory_8h" name="measurement_operator_factory.h" local="yes" imported="no">purify/measurement_operator_factory.h</includes>
    <includes id="wavelet__operator__factory_8h" name="wavelet_operator_factory.h" local="yes" imported="no">purify/wavelet_operator_factory.h</includes>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>dirty_visibilities</name>
      <anchorfile>mpi__algo__factory_8cc.html</anchorfile>
      <anchor>afdd6549b257bd91a43bf98d8654cf0cd</anchor>
      <arglist>(const std::vector&lt; std::string &gt; &amp;names)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>dirty_visibilities</name>
      <anchorfile>mpi__algo__factory_8cc.html</anchorfile>
      <anchor>a571c9ca1effca2d32f82a7ffabd6c30e</anchor>
      <arglist>(const std::vector&lt; std::string &gt; &amp;names, sopt::mpi::Communicator const &amp;comm)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>mpi__algo__factory_8cc.html</anchorfile>
      <anchor>a0802dd3dd78d5485c8a178c683a45b31</anchor>
      <arglist>(&quot;Serial vs. Serial with MPI PADMM&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>mpi__algo__factory_8cc.html</anchorfile>
      <anchor>a31fed0709bdf98aea0ff6a0410cad468</anchor>
      <arglist>(&quot;Serial vs. Serial with MPI Forward Backward&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>mpi_measurement_factory.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/mpi__measurement__factory_8cc</filename>
    <includes id="distribute_8h" name="distribute.h" local="yes" imported="no">purify/distribute.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="measurement__operator__factory_8h" name="measurement_operator_factory.h" local="yes" imported="no">purify/measurement_operator_factory.h</includes>
    <includes id="mpi__utilities_8h" name="mpi_utilities.h" local="yes" imported="no">purify/mpi_utilities.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>mpi__measurement__factory_8cc.html</anchorfile>
      <anchor>a4b4144b317ffdd35b81e65d5d926149b</anchor>
      <arglist>(&quot;Serial vs Distributed Operator&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>mpi__measurement__factory_8cc.html</anchorfile>
      <anchor>a6f0552812c9180a64360dbba3608a5a3</anchor>
      <arglist>(&quot;GPU Serial vs Distributed Operator&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>mpi_measurement_operator.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/mpi__measurement__operator_8cc</filename>
    <includes id="distribute_8h" name="distribute.h" local="yes" imported="no">purify/distribute.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="mpi__utilities_8h" name="mpi_utilities.h" local="yes" imported="no">purify/mpi_utilities.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <includes id="wproj__operators_8h" name="wproj_operators.h" local="yes" imported="no">purify/wproj_operators.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>mpi__measurement__operator_8cc.html</anchorfile>
      <anchor>a4b4144b317ffdd35b81e65d5d926149b</anchor>
      <arglist>(&quot;Serial vs Distributed Operator&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>mpi__measurement__operator_8cc.html</anchorfile>
      <anchor>a0fca63c60b8ff70de881734a067f1644</anchor>
      <arglist>(&quot;Serial vs Distributed Fourier Grid Operator&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>mpi__measurement__operator_8cc.html</anchorfile>
      <anchor>a4bbcc6111afbeac899814b202f828b81</anchor>
      <arglist>(&quot;Serial vs Distributed Fourier Grid Operator weighted&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>mpi__measurement__operator_8cc.html</anchorfile>
      <anchor>a6a5ddff379ba73ac79c381e58ddb18a0</anchor>
      <arglist>(&quot;Serial vs Distributed Operator Radial WProjection&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>mpi_read_measurements.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/mpi__read__measurements_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="read__measurements_8h" name="read_measurements.h" local="yes" imported="no">purify/read_measurements.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>mpi__read__measurements_8cc.html</anchorfile>
      <anchor>afd7df3df7aae562f92801f2493f01cdb</anchor>
      <arglist>(&quot;uvfits&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>mpi_wavelet_factory.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/mpi__wavelet__factory_8cc</filename>
    <includes id="distribute_8h" name="distribute.h" local="yes" imported="no">purify/distribute.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="mpi__utilities_8h" name="mpi_utilities.h" local="yes" imported="no">purify/mpi_utilities.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <includes id="wavelet__operator__factory_8h" name="wavelet_operator_factory.h" local="yes" imported="no">purify/wavelet_operator_factory.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>mpi__wavelet__factory_8cc.html</anchorfile>
      <anchor>a4b4144b317ffdd35b81e65d5d926149b</anchor>
      <arglist>(&quot;Serial vs Distributed Operator&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>operators_gpu.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/operators__gpu_8cc</filename>
    <includes id="operators__gpu_8h" name="operators_gpu.h" local="yes" imported="no">purify/operators_gpu.h</includes>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="kernels_8h" name="kernels.h" local="yes" imported="no">purify/kernels.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="wproj__operators__gpu_8h" name="wproj_operators_gpu.h" local="yes" imported="no">purify/wproj_operators_gpu.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>operators__gpu_8cc.html</anchorfile>
      <anchor>a9d26e1a7b9b8c76dfecfebc07ce192a3</anchor>
      <arglist>(&quot;GPU Operators&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>operators__gpu_8cc.html</anchorfile>
      <anchor>a79bae0670635c162df98f14160b9e02a</anchor>
      <arglist>(&quot;wprojection&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>parallel_mpi_utilities.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/parallel__mpi__utilities_8cc</filename>
    <includes id="mpi__utilities_8h" name="mpi_utilities.h" local="yes" imported="no">purify/mpi_utilities.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>parallel__mpi__utilities_8cc.html</anchorfile>
      <anchor>a79734554ca8304e2dfde5b1873ce758f</anchor>
      <arglist>(&quot;Distribution of visibility data&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>parser_test.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/parser__test_8cc</filename>
    <includes id="yaml-parser_8h" name="yaml-parser.h" local="yes" imported="no">purify/yaml-parser.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>parser__test_8cc.html</anchorfile>
      <anchor>a677346f6cad3e0e7401ddb99a1bcd31b</anchor>
      <arglist>(&quot;Yaml parser and setting variables test&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>purify_fitsio.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/purify__fitsio_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>purify__fitsio_8cc.html</anchorfile>
      <anchor>ae3a89f39d15278cccde69ad427cf5032</anchor>
      <arglist>(&quot;Purify fitsio&quot;, &quot;[readwrite]&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>purify__fitsio_8cc.html</anchorfile>
      <anchor>a41aa9bb69af86b8437aa0ba5a7d744b1</anchor>
      <arglist>(&quot;readwrite2dheader&quot;, &quot;purify fitsio&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>purify__fitsio_8cc.html</anchorfile>
      <anchor>a0ab6184a9e385b586d075add297ac081</anchor>
      <arglist>(&quot;readwrite3d&quot;, &quot;purify fitsio&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>purify__fitsio_8cc.html</anchorfile>
      <anchor>a49482f67f1396652382d07a2400d5e19</anchor>
      <arglist>(&quot;readwrite3dheader&quot;, &quot;purify fitsio&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>purify__fitsio_8cc.html</anchorfile>
      <anchor>ab77a784e7f1b4cd405c7192bfeec1f23</anchor>
      <arglist>(&quot;readwrite3dwith2d&quot;, &quot;purify fitsio&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>purify__fitsio_8cc.html</anchorfile>
      <anchor>a972d476e7620dfb48b36927009ca49d9</anchor>
      <arglist>(&quot;readwrite3dheaderwith2d&quot;, &quot;purify fitsio&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>purify__fitsio_8cc.html</anchorfile>
      <anchor>af7a0b166e7d3259f6b7874a0cca6e677</anchor>
      <arglist>(&quot;header&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>serial_vs_parallel_padmm.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/serial__vs__parallel__padmm_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="distribute_8h" name="distribute.h" local="yes" imported="no">purify/distribute.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="mpi__utilities_8h" name="mpi_utilities.h" local="yes" imported="no">purify/mpi_utilities.h</includes>
    <includes id="operators_8h" name="operators.h" local="yes" imported="no">purify/operators.h</includes>
    <includes id="pfitsio_8h" name="pfitsio.h" local="yes" imported="no">purify/pfitsio.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>dirty_visibilities</name>
      <anchorfile>serial__vs__parallel__padmm_8cc.html</anchorfile>
      <anchor>afe5e952b59a169f29c862ee9709cc251</anchor>
      <arglist>(t_uint number_of_vis=10, t_uint width=20, t_uint height=20, t_uint over_sample=2, t_real ISNR=30)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>dirty_visibilities</name>
      <anchorfile>serial__vs__parallel__padmm_8cc.html</anchorfile>
      <anchor>a584149d4457b29d7dfaf7c83af0e0460</anchor>
      <arglist>(sopt::mpi::Communicator const &amp;comm, t_uint number_of_vis=10, t_uint width=20, t_uint height=20, t_uint over_sample=2, t_real ISNR=30)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>serial__vs__parallel__padmm_8cc.html</anchorfile>
      <anchor>a330b35a606144274f6402c3c4e8065d5</anchor>
      <arglist>(&quot;Serial vs. Parallel PADMM with random coverage.&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>sparse.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/sparse_8cc</filename>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>sparse_8cc.html</anchorfile>
      <anchor>a2325734915f1ce4eef1aba48574a87f9</anchor>
      <arglist>(&quot;sparse [iterator]&quot;, &quot;[iterator]&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>utils.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/utils_8cc</filename>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>utils_8cc.html</anchorfile>
      <anchor>afc4869ab50066bb61ade496ed27f231a</anchor>
      <arglist>(&quot;utilities [mod]&quot;, &quot;[mod]&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>utils_8cc.html</anchorfile>
      <anchor>a11c2c2b1d50a1f9cd691fc6837f6f45e</anchor>
      <arglist>(&quot;utilities [reshape]&quot;, &quot;[reshape]&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>utils_8cc.html</anchorfile>
      <anchor>a0a9bc2b672a2ee9e1407ff6fea0d4103</anchor>
      <arglist>(&quot;utilities [variance]&quot;, &quot;[variance]&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>utils_8cc.html</anchorfile>
      <anchor>ad1ffa5afe84abecd4e520989327863dd</anchor>
      <arglist>(&quot;utilities [median]&quot;, &quot;[median]&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>utils_8cc.html</anchorfile>
      <anchor>a985d9c995a6fee9f9982959983a2ca73</anchor>
      <arglist>(&quot;utilities [read_write_vis]&quot;, &quot;[read_write_vis]&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>utils_8cc.html</anchorfile>
      <anchor>a5b956b78bef1ca0ca6dd8d4a02dbc75b</anchor>
      <arglist>(&quot;read_mutiple_vis&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>utils_8cc.html</anchorfile>
      <anchor>a78fd7fc925c4e732904a37783a8a709d</anchor>
      <arglist>(&quot;utilities [file exists]&quot;, &quot;[file exists]&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>utils_8cc.html</anchorfile>
      <anchor>ad5f06432fef7c5000dd74bf41443d2b2</anchor>
      <arglist>(&quot;utilities [fit_fwhm]&quot;, &quot;[fit_fwhm]&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>utils_8cc.html</anchorfile>
      <anchor>a3ab98016d257b793b922b1bf341da1a1</anchor>
      <arglist>(&quot;utilities [sparse multiply]&quot;, &quot;[sparse multiply]&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>utils_8cc.html</anchorfile>
      <anchor>a0d4ce22c6bcead9789fe0a7bca7a7044</anchor>
      <arglist>(&quot;generate_baseline&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>utils_8cc.html</anchorfile>
      <anchor>a252b0fc313e8fb83b83e0c9e0720dd93</anchor>
      <arglist>(&quot;conjugate symmetry&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>wavelet_factory.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/wavelet__factory_8cc</filename>
    <includes id="distribute_8h" name="distribute.h" local="yes" imported="no">purify/distribute.h</includes>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="mpi__utilities_8h" name="mpi_utilities.h" local="yes" imported="no">purify/mpi_utilities.h</includes>
    <includes id="purify_2utilities_8h" name="utilities.h" local="yes" imported="no">purify/utilities.h</includes>
    <includes id="wavelet__operator__factory_8h" name="wavelet_operator_factory.h" local="yes" imported="no">purify/wavelet_operator_factory.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>wavelet__factory_8cc.html</anchorfile>
      <anchor>a0b7e6e1b52564bde4b587c503b079621</anchor>
      <arglist>(&quot;Wavelet Factory Operator&quot;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>wkernel.cc</name>
    <path>/mydata/cpp/tests/</path>
    <filename>http://astro-informatics.github.io/purify/wkernel_8cc</filename>
    <includes id="logging_8h" name="logging.h" local="yes" imported="no">purify/logging.h</includes>
    <includes id="types_8h" name="types.h" local="yes" imported="no">purify/types.h</includes>
    <includes id="kernels_8h" name="kernels.h" local="yes" imported="no">purify/kernels.h</includes>
    <includes id="wide__field__utilities_8h" name="wide_field_utilities.h" local="yes" imported="no">purify/wide_field_utilities.h</includes>
    <includes id="wkernel__integration_8h" name="wkernel_integration.h" local="yes" imported="no">purify/wkernel_integration.h</includes>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>wkernel_8cc.html</anchorfile>
      <anchor>a27e5030eefe2cb8dffefaf0816ea1a50</anchor>
      <arglist>(&quot;test transform kernels&quot;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_CASE</name>
      <anchorfile>wkernel_8cc.html</anchorfile>
      <anchor>a9d9e376580025148ebf6a60a470f58bd</anchor>
      <arglist>(&quot;calculating zero&quot;)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>purify::casa::MeasurementSet::ChannelWrapper</name>
    <filename>http://astro-informatics.github.io/purify/classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>Sigma</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>a8a3486e632c38521b251686813425338</anchor>
      <arglist></arglist>
      <enumvalue file="classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html" anchor="a8a3486e632c38521b251686813425338a7905c9b3eda793da18fade6e791b2183">OVERALL</enumvalue>
      <enumvalue file="classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html" anchor="a8a3486e632c38521b251686813425338a63225cbec06076dbc18e99cd79bdfce1">SPECTRUM</enumvalue>
    </member>
    <member kind="function">
      <type></type>
      <name>ChannelWrapper</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>ad187c64f5f270cdaa0847fa667f2fea1</anchor>
      <arglist>(t_uint channel, MeasurementSet const &amp;ms, std::string const &amp;filter=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>t_uint</type>
      <name>channel</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>ae03dfd380e1091a7ea589f09ca9c09be</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>PURIFY_MACRO</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>a80e4cc9886d6b2bcfb8e5d5e94fbeb74</anchor>
      <arglist>(u, 0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>PURIFY_MACRO</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>a8277f96dcebd1a43ae2f3391e9fd869e</anchor>
      <arglist>(v, 1)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>PURIFY_MACRO</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>a4a36b2f546ae2732b25ee90468c801b8</anchor>
      <arglist>(w, 2)</arglist>
    </member>
    <member kind="function">
      <type>t_uint</type>
      <name>size</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>acec0a8ea5ee5e85181be8b516d48075e</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_int &gt;</type>
      <name>field_ids</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>a1fa7d891f0d3ae941e541405078a055e</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_int &gt;</type>
      <name>data_desc_id</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>ac43d590e3d118ed320736233916ae0ba</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Direction</type>
      <name>direction</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>aa5569b378ac711c015e4a788667fc87f</anchor>
      <arglist>(t_real tolerance=1e-8) const</arglist>
    </member>
    <member kind="function">
      <type>Direction::Scalar</type>
      <name>right_ascension</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>a6cdae8611cc0134f75f79148f0e773ce</anchor>
      <arglist>(t_real tolerance=1e-8) const</arglist>
    </member>
    <member kind="function">
      <type>Direction::Scalar</type>
      <name>declination</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>a5cb7ea7995e816866787630e7d85bb97</anchor>
      <arglist>(t_real tolerance=1e-8) const</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_real &gt;</type>
      <name>raw_frequencies</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>ab45d707152115d27dff9df1a13507510</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>PURIFY_MACRO</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>a20810bbaaa69fb5dc46bd00d680cc776</anchor>
      <arglist>(I)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>PURIFY_MACRO</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>aecf1e102351c4443391e83108447d324</anchor>
      <arglist>(Q)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>PURIFY_MACRO</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>a8481c70475dceb0085ed0e66f2259322</anchor>
      <arglist>(U)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>PURIFY_MACRO</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>a55760b81ac37984a87cbdd92d836f138</anchor>
      <arglist>(V)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>PURIFY_MACRO</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>a2adbe050feef60b0bcd53c6ff479ca76</anchor>
      <arglist>(LL)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>PURIFY_MACRO</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>a75481006650902e412ac953f0637b529</anchor>
      <arglist>(RR)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>PURIFY_MACRO</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>aeacd4b95edbb7a98c897b540245a37c8</anchor>
      <arglist>(LR)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>PURIFY_MACRO</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>a15f5b5e21ddf86d717902f4b929b59ea</anchor>
      <arglist>(RL)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>PURIFY_MACRO</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>a2850ff09558b971c237fa029e277b754</anchor>
      <arglist>(XX)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>PURIFY_MACRO</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>ad423de922c3e75b7ee82ce54858cb2b1</anchor>
      <arglist>(YY)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>PURIFY_MACRO</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>a95e3108585da44c13290fd4de4f64ede</anchor>
      <arglist>(XY)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>PURIFY_MACRO</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>a8d5eef214232bc332d3d44edb25706de</anchor>
      <arglist>(YX)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_real &gt;</type>
      <name>frequencies</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>aff1613d2e38707692e277f8e5b1f3352</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_real &gt;</type>
      <name>width</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>ad6f925563470cd2fed3221be936038f7</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_real &gt;</type>
      <name>effective_noise_bandwidth</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>ab29808c0d2b0fdaa5668e74692a768db</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_real &gt;</type>
      <name>resolution</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>aa4c9ab87c07069504f7a733ff9511565</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>is_valid</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1ChannelWrapper.html</anchorfile>
      <anchor>afc12a6cdd8db10b9bcc921e01c95ff24</anchor>
      <arglist>() const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>purify::casa::MeasurementSet::const_iterator</name>
    <filename>http://astro-informatics.github.io/purify/classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</filename>
    <member kind="typedef">
      <type>MeasurementSet::ChannelWrapper</type>
      <name>value_type</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</anchorfile>
      <anchor>a4068a25accca01f0aa17e487cd777203</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>std::shared_ptr&lt; value_type const  &gt;</type>
      <name>pointer</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</anchorfile>
      <anchor>aa14fc508f069b9dd36a4530d4a92735a</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>value_type const  &amp;</type>
      <name>reference</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</anchorfile>
      <anchor>a297d8194c6e9116a5cdcfea2ba67db49</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>t_int</type>
      <name>difference_type</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</anchorfile>
      <anchor>a9a5174b3d3ee12335f2a7d6293aeaf70</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>const_iterator</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</anchorfile>
      <anchor>a37fb894b0f681396a5e172093967316f</anchor>
      <arglist>(t_int channel, MeasurementSet const &amp;ms, std::string const &amp;filter=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>pointer</type>
      <name>operator-&gt;</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</anchorfile>
      <anchor>a77728273a0e0bf5bc351127822a61342</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>reference</type>
      <name>operator*</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</anchorfile>
      <anchor>a4a5643250ccf163720b9264a43431edf</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>const_iterator &amp;</type>
      <name>operator++</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</anchorfile>
      <anchor>a74b570c26c2cf01a34667832c85b9ae9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>const_iterator</type>
      <name>operator++</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</anchorfile>
      <anchor>a2c35943bd6f0e78466695ef338ca9ec8</anchor>
      <arglist>(int)</arglist>
    </member>
    <member kind="function">
      <type>const_iterator &amp;</type>
      <name>operator+=</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</anchorfile>
      <anchor>a5759c28a4cd70649544eb47744cb1edf</anchor>
      <arglist>(difference_type n)</arglist>
    </member>
    <member kind="function">
      <type>const_iterator &amp;</type>
      <name>operator-=</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</anchorfile>
      <anchor>ad47b99677c6fab4ca743dbe321c29f9d</anchor>
      <arglist>(difference_type n)</arglist>
    </member>
    <member kind="function">
      <type>const_iterator</type>
      <name>operator+</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</anchorfile>
      <anchor>a9957e2b358d897c48a031b9c9c3fc068</anchor>
      <arglist>(difference_type n) const</arglist>
    </member>
    <member kind="function">
      <type>const_iterator</type>
      <name>operator-</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</anchorfile>
      <anchor>a240226c83353f2ed64dd4ba0e772da3a</anchor>
      <arglist>(difference_type n) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</anchorfile>
      <anchor>ab3a5966f073e100a2bc2168387cb7e26</anchor>
      <arglist>(const_iterator const &amp;c) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;=</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</anchorfile>
      <anchor>abd0830168c43abb34f5173c11145082a</anchor>
      <arglist>(const_iterator const &amp;c) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</anchorfile>
      <anchor>ad2d240a0d8b03b72b759d8e556725a11</anchor>
      <arglist>(const_iterator const &amp;c) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;=</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</anchorfile>
      <anchor>a240ed9126bf531dbe45b2b5c31ab52b0</anchor>
      <arglist>(const_iterator const &amp;c) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</anchorfile>
      <anchor>affd5eb133f030207266ea02bc09926d6</anchor>
      <arglist>(const_iterator const &amp;c) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator!=</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</anchorfile>
      <anchor>a2a225198bff9190fa3c96f7164dd9453</anchor>
      <arglist>(const_iterator const &amp;c) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>same_measurement_set</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet_1_1const__iterator.html</anchorfile>
      <anchor>ab84f8fb9ec0c0cf9c4be1770e471b82f</anchor>
      <arglist>(const_iterator const &amp;c) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DegridOperatorAdjointFixture</name>
    <filename>http://astro-informatics.github.io/purify/classDegridOperatorAdjointFixture.html</filename>
    <base>DegridOperatorFixture</base>
    <base>DegridOperatorFixture</base>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>updateImage</name>
      <anchorfile>classDegridOperatorAdjointFixture.html</anchorfile>
      <anchor>ac72f7e9d06085ae4d9c44c0e0e168ef2</anchor>
      <arglist>(t_uint newSize)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>updateImage</name>
      <anchorfile>classDegridOperatorAdjointFixture.html</anchorfile>
      <anchor>ac72f7e9d06085ae4d9c44c0e0e168ef2</anchor>
      <arglist>(t_uint newSize)</arglist>
    </member>
    <member kind="variable">
      <type>Vector&lt; t_complex &gt;</type>
      <name>m_image</name>
      <anchorfile>classDegridOperatorAdjointFixture.html</anchorfile>
      <anchor>ac60d9829ff119663d6d96304b428bc1b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DegridOperatorAdjointFixtureDistr</name>
    <filename>http://astro-informatics.github.io/purify/classDegridOperatorAdjointFixtureDistr.html</filename>
    <base>DegridOperatorAdjointFixturePar</base>
    <member kind="function" virtualness="virtual">
      <type>virtual std::shared_ptr&lt; sopt::LinearTransform&lt; Vector&lt; t_complex &gt; &gt; const  &gt;</type>
      <name>measurementOperator</name>
      <anchorfile>classDegridOperatorAdjointFixtureDistr.html</anchorfile>
      <anchor>a41e23337cd1ca85d57222685ae0eebf5</anchor>
      <arglist>(t_real cellsize, bool w_term)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DegridOperatorAdjointFixtureMPI</name>
    <filename>http://astro-informatics.github.io/purify/classDegridOperatorAdjointFixtureMPI.html</filename>
    <base>DegridOperatorAdjointFixturePar</base>
    <member kind="function" virtualness="virtual">
      <type>virtual std::shared_ptr&lt; sopt::LinearTransform&lt; Vector&lt; t_complex &gt; &gt; const  &gt;</type>
      <name>measurementOperator</name>
      <anchorfile>classDegridOperatorAdjointFixtureMPI.html</anchorfile>
      <anchor>a5f6ef767a3aca9287ea00010a58e6355</anchor>
      <arglist>(t_real cellsize, bool w_term)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DegridOperatorAdjointFixturePar</name>
    <filename>http://astro-informatics.github.io/purify/classDegridOperatorAdjointFixturePar.html</filename>
    <base>DegridOperatorFixturePar</base>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>updateImage</name>
      <anchorfile>classDegridOperatorAdjointFixturePar.html</anchorfile>
      <anchor>aad85548e8bcf4772e3b6eaf1fbb46211</anchor>
      <arglist>(t_uint newSize)</arglist>
    </member>
    <member kind="variable">
      <type>Vector&lt; t_complex &gt;</type>
      <name>m_image</name>
      <anchorfile>classDegridOperatorAdjointFixturePar.html</anchorfile>
      <anchor>aa6a7208ff1a9940d141fea521e9d2582</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DegridOperatorCtorFixturePar</name>
    <filename>http://astro-informatics.github.io/purify/classDegridOperatorCtorFixturePar.html</filename>
    <member kind="function">
      <type>void</type>
      <name>SetUp</name>
      <anchorfile>classDegridOperatorCtorFixturePar.html</anchorfile>
      <anchor>a459dd3ab089e71fd83f9ed503f76a3c9</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>TearDown</name>
      <anchorfile>classDegridOperatorCtorFixturePar.html</anchorfile>
      <anchor>af6efd1bd4e930ee8fe1e980b60d03644</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_counter</name>
      <anchorfile>classDegridOperatorCtorFixturePar.html</anchorfile>
      <anchor>ae88863b3e8a8f20e5360655b933f6a3f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>sopt::mpi::Communicator</type>
      <name>m_world</name>
      <anchorfile>classDegridOperatorCtorFixturePar.html</anchorfile>
      <anchor>a53266a28b6e43e41a82c80dc8b0dc973</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_imsizex</name>
      <anchorfile>classDegridOperatorCtorFixturePar.html</anchorfile>
      <anchor>adc067fa9305724d7a4ae5689f583d0cf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_imsizey</name>
      <anchorfile>classDegridOperatorCtorFixturePar.html</anchorfile>
      <anchor>a43a0ba9f5f940baab6ecc4ac1fdb95ae</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>utilities::vis_params</type>
      <name>m_uv_data</name>
      <anchorfile>classDegridOperatorCtorFixturePar.html</anchorfile>
      <anchor>a5373ce2f02de6bb2d7080b653dac84c1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_real</type>
      <name>m_cellsize</name>
      <anchorfile>classDegridOperatorCtorFixturePar.html</anchorfile>
      <anchor>a5e34a9555e3ed67992611cfdb290dcdd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>m_w_term</name>
      <anchorfile>classDegridOperatorCtorFixturePar.html</anchorfile>
      <anchor>a32ed96a79ff87a33ae546d8231ce76ff</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DegridOperatorDirectFixture</name>
    <filename>http://astro-informatics.github.io/purify/classDegridOperatorDirectFixture.html</filename>
    <base>DegridOperatorFixture</base>
    <base>DegridOperatorFixture</base>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>updateImage</name>
      <anchorfile>classDegridOperatorDirectFixture.html</anchorfile>
      <anchor>a6a1bef10cf272b0fbf75274d143096e8</anchor>
      <arglist>(t_uint newSize)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>updateImage</name>
      <anchorfile>classDegridOperatorDirectFixture.html</anchorfile>
      <anchor>a6a1bef10cf272b0fbf75274d143096e8</anchor>
      <arglist>(t_uint newSize)</arglist>
    </member>
    <member kind="variable">
      <type>Image&lt; t_complex &gt;</type>
      <name>m_image</name>
      <anchorfile>classDegridOperatorDirectFixture.html</anchorfile>
      <anchor>ae1f8c8869ad50598e64789408fd32653</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DegridOperatorDirectFixtureDistr</name>
    <filename>http://astro-informatics.github.io/purify/classDegridOperatorDirectFixtureDistr.html</filename>
    <base>DegridOperatorDirectFixturePar</base>
    <member kind="function" virtualness="virtual">
      <type>virtual std::shared_ptr&lt; sopt::LinearTransform&lt; Vector&lt; t_complex &gt; &gt; const  &gt;</type>
      <name>measurementOperator</name>
      <anchorfile>classDegridOperatorDirectFixtureDistr.html</anchorfile>
      <anchor>a7774fb0d0fe74f485d2dcac5c3da5c2d</anchor>
      <arglist>(t_real cellsize, bool w_term)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DegridOperatorDirectFixtureMPI</name>
    <filename>http://astro-informatics.github.io/purify/classDegridOperatorDirectFixtureMPI.html</filename>
    <base>DegridOperatorDirectFixturePar</base>
    <member kind="function" virtualness="virtual">
      <type>virtual std::shared_ptr&lt; sopt::LinearTransform&lt; Vector&lt; t_complex &gt; &gt; const  &gt;</type>
      <name>measurementOperator</name>
      <anchorfile>classDegridOperatorDirectFixtureMPI.html</anchorfile>
      <anchor>a46eb8a62e985bc3fd6224d0ef5d2329d</anchor>
      <arglist>(t_real cellsize, bool w_term)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DegridOperatorDirectFixturePar</name>
    <filename>http://astro-informatics.github.io/purify/classDegridOperatorDirectFixturePar.html</filename>
    <base>DegridOperatorFixturePar</base>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>updateImage</name>
      <anchorfile>classDegridOperatorDirectFixturePar.html</anchorfile>
      <anchor>a4a91003ffc8d179cf3cadb3c430fa7cc</anchor>
      <arglist>(t_uint newSize)</arglist>
    </member>
    <member kind="variable">
      <type>Image&lt; t_complex &gt;</type>
      <name>m_image</name>
      <anchorfile>classDegridOperatorDirectFixturePar.html</anchorfile>
      <anchor>a47b6672e653bb0cff6eb143fa4a22452</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DegridOperatorFixture</name>
    <filename>http://astro-informatics.github.io/purify/classDegridOperatorFixture.html</filename>
    <member kind="function">
      <type>void</type>
      <name>SetUp</name>
      <anchorfile>classDegridOperatorFixture.html</anchorfile>
      <anchor>a55d320618ffba8209c78119e909838f7</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>TearDown</name>
      <anchorfile>classDegridOperatorFixture.html</anchorfile>
      <anchor>a4f7a12464eb325f6508899fd6f40b68f</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>updateImage</name>
      <anchorfile>classDegridOperatorFixture.html</anchorfile>
      <anchor>a086983a1783bc67e4b63a6f5603e711d</anchor>
      <arglist>(t_uint newSize)=0</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>SetUp</name>
      <anchorfile>classDegridOperatorFixture.html</anchorfile>
      <anchor>a55d320618ffba8209c78119e909838f7</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>TearDown</name>
      <anchorfile>classDegridOperatorFixture.html</anchorfile>
      <anchor>a4f7a12464eb325f6508899fd6f40b68f</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>updateImage</name>
      <anchorfile>classDegridOperatorFixture.html</anchorfile>
      <anchor>a086983a1783bc67e4b63a6f5603e711d</anchor>
      <arglist>(t_uint newSize)=0</arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_counter</name>
      <anchorfile>classDegridOperatorFixture.html</anchorfile>
      <anchor>a6b9a9c279267cc240773f856e121b042</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_imsizex</name>
      <anchorfile>classDegridOperatorFixture.html</anchorfile>
      <anchor>a93a66e43d3032dea889f689c9e7ba695</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_imsizey</name>
      <anchorfile>classDegridOperatorFixture.html</anchorfile>
      <anchor>a9299c9507d6ad4e6d8486cd39394c6ed</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>utilities::vis_params</type>
      <name>m_uv_data</name>
      <anchorfile>classDegridOperatorFixture.html</anchorfile>
      <anchor>a97a9f9321be5678e08da21b254b4361c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_kernel</name>
      <anchorfile>classDegridOperatorFixture.html</anchorfile>
      <anchor>a598bca021971bdf025d86f4175cffc5f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::shared_ptr&lt; sopt::LinearTransform&lt; Vector&lt; t_complex &gt; &gt; const  &gt;</type>
      <name>m_degridOperator</name>
      <anchorfile>classDegridOperatorFixture.html</anchorfile>
      <anchor>af147e6a5f170b53837f3ae187caefea8</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DegridOperatorFixturePar</name>
    <filename>http://astro-informatics.github.io/purify/classDegridOperatorFixturePar.html</filename>
    <member kind="function">
      <type>void</type>
      <name>SetUp</name>
      <anchorfile>classDegridOperatorFixturePar.html</anchorfile>
      <anchor>a9e964f24b22179e00dfa6143e49212fe</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>TearDown</name>
      <anchorfile>classDegridOperatorFixturePar.html</anchorfile>
      <anchor>a00af23a6d249f7277bf14a877b692774</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>updateImage</name>
      <anchorfile>classDegridOperatorFixturePar.html</anchorfile>
      <anchor>ae7781ca163697a08e6d3e314fbd78cdf</anchor>
      <arglist>(t_uint newSize)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual std::shared_ptr&lt; sopt::LinearTransform&lt; Vector&lt; t_complex &gt; &gt; const  &gt;</type>
      <name>measurementOperator</name>
      <anchorfile>classDegridOperatorFixturePar.html</anchorfile>
      <anchor>a65c25a6d515816dd70d76d90c543ee42</anchor>
      <arglist>(t_real cellsize, bool w_term)=0</arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_counter</name>
      <anchorfile>classDegridOperatorFixturePar.html</anchorfile>
      <anchor>ad9e9fc264516913f62927b45e4f1b398</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>sopt::mpi::Communicator</type>
      <name>m_world</name>
      <anchorfile>classDegridOperatorFixturePar.html</anchorfile>
      <anchor>a5f0270d50a932b4e754d5d21452eb3d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_kernel</name>
      <anchorfile>classDegridOperatorFixturePar.html</anchorfile>
      <anchor>a20418a5c5d73023c154cfa8c636676b3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_imsizex</name>
      <anchorfile>classDegridOperatorFixturePar.html</anchorfile>
      <anchor>ae96a2deb375196d6bd1dfa0566b3c2aa</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_imsizey</name>
      <anchorfile>classDegridOperatorFixturePar.html</anchorfile>
      <anchor>a9b603eea43789a422662bd879d94b979</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>utilities::vis_params</type>
      <name>m_uv_data</name>
      <anchorfile>classDegridOperatorFixturePar.html</anchorfile>
      <anchor>ae3b7d88e1d6b2997393e09bddc3c22a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::shared_ptr&lt; sopt::LinearTransform&lt; Vector&lt; t_complex &gt; &gt; const  &gt;</type>
      <name>m_degridOperator</name>
      <anchorfile>classDegridOperatorFixturePar.html</anchorfile>
      <anchor>ab6dc428495da4c064bb7c6b493519b09</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>FFTOperatorFixture</name>
    <filename>http://astro-informatics.github.io/purify/classFFTOperatorFixture.html</filename>
    <member kind="function">
      <type>void</type>
      <name>SetUp</name>
      <anchorfile>classFFTOperatorFixture.html</anchorfile>
      <anchor>a1ead0616a6cc0a6a3237817bd0f7d34d</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>TearDown</name>
      <anchorfile>classFFTOperatorFixture.html</anchorfile>
      <anchor>a6d1795aa2c0cdb53e9f75f0f91f8458f</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_counter</name>
      <anchorfile>classFFTOperatorFixture.html</anchorfile>
      <anchor>a5ad1f577b3fa18d44007f6b6f571cfd3</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>GridOperatorFixture</name>
    <filename>http://astro-informatics.github.io/purify/classGridOperatorFixture.html</filename>
    <member kind="function">
      <type>void</type>
      <name>SetUp</name>
      <anchorfile>classGridOperatorFixture.html</anchorfile>
      <anchor>a5a2094de00d58e40c1f97cf3cc6b2450</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>TearDown</name>
      <anchorfile>classGridOperatorFixture.html</anchorfile>
      <anchor>a1b6c33d2ee57385bd291aaa0afe029ef</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_counter</name>
      <anchorfile>classGridOperatorFixture.html</anchorfile>
      <anchor>a4f639b191fc9bf7bbc20def5efc9a98d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>M</name>
      <anchorfile>classGridOperatorFixture.html</anchorfile>
      <anchor>aeb4d1c034d31848cc9957e1b7fbac90a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_imsizey</name>
      <anchorfile>classGridOperatorFixture.html</anchorfile>
      <anchor>af88da1763ce9a90cc2da00ae27279507</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_imsizex</name>
      <anchorfile>classGridOperatorFixture.html</anchorfile>
      <anchor>aac9f05941316de1b375d9fc3e43edbe8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::tuple&lt; sopt::OperatorFunction&lt; Vector&lt; t_complex &gt; &gt;, sopt::OperatorFunction&lt; Vector&lt; t_complex &gt; &gt; &gt;</type>
      <name>Gop</name>
      <anchorfile>classGridOperatorFixture.html</anchorfile>
      <anchor>aa7fc9410200a0dad082e1cface93d9aa</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>purify::pfitsio::header_params</name>
    <filename>http://astro-informatics.github.io/purify/structpurify_1_1pfitsio_1_1header__params.html</filename>
    <member kind="function">
      <type></type>
      <name>header_params</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>aa24b31fe1bd20cb9f15602f43a73603a</anchor>
      <arglist>(const std::string &amp;fits_name_, const std::string &amp;pix_units_, const t_real channels_total_, const t_real ra_, const t_real dec_, const stokes pol, const t_real cellx_, const t_real celly_, const t_real mean_frequency_, const t_real channel_width_, const t_uint niters_, const bool hasconverged_, const t_real relative_variation_, const t_real residual_convergence_, const t_real epsilon_)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>header_params</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>a7155b9acf45289b4b71e0877547a9599</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>a4a140ca20df672acb506bdaf51de522b</anchor>
      <arglist>(const header_params &amp;h2) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator!=</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>a4043104fe3255b3d36fef93506fc4943</anchor>
      <arglist>(const header_params &amp;h2) const</arglist>
    </member>
    <member kind="variable">
      <type>std::string</type>
      <name>fits_name</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>a52ed16fe9c9d5deda4f7a32862fa5f1c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_real</type>
      <name>mean_frequency</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>a07efb2adb2f83d98627f19fcb832b9a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_real</type>
      <name>cell_x</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>a19cfa17aa17b9eafc866a9dad848b669</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_real</type>
      <name>cell_y</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>a3a8abeaf73d42fd1866b2bb704ac996e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_real</type>
      <name>ra</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>a47581fb8f774890dd5dc3db4ff919fae</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_real</type>
      <name>dec</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>ae3105f99dd7cd7a1549a4d7f5a7b96cc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_int</type>
      <name>pix_ref_x</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>afdedf9dd7f0f89c3a9c76d4768f65084</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_int</type>
      <name>pix_ref_y</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>a85e2b3521ced5aa63b4978365591c3cf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::string</type>
      <name>pix_units</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>a9fca5b73fa4d5d69064001d57af4023b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_real</type>
      <name>channels_total</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>adcc806744c8a4ee546a18712b1827e8a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_real</type>
      <name>channel_width</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>aaa8f8fa48c1860ef5a830e4a86f04e25</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_real</type>
      <name>polarisation</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>a932f01a2e9f39a624f97c54703a25d1c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_int</type>
      <name>niters</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>aa67b53c09939f617f9ac564e17f95d3b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>hasconverged</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>a2351bd42659575e0fbcdbfda72d6f13b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_real</type>
      <name>relative_variation</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>a37f4a27934271b097078a1626a53b535</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_real</type>
      <name>residual_convergence</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>a8c3aa97823dd98fc90bcb13a5274e882</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_real</type>
      <name>epsilon</name>
      <anchorfile>structpurify_1_1pfitsio_1_1header__params.html</anchorfile>
      <anchor>a60b39c97f797d267a277d809b98ed22b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>purify::IndexMapping</name>
    <filename>http://astro-informatics.github.io/purify/classpurify_1_1IndexMapping.html</filename>
    <member kind="function">
      <type></type>
      <name>IndexMapping</name>
      <anchorfile>classpurify_1_1IndexMapping.html</anchorfile>
      <anchor>a3cf84d8eb0d9b3b9c7b7ed08cca787b6</anchor>
      <arglist>(const std::vector&lt; t_int &gt; &amp;indices, const t_int N, const t_int start=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>IndexMapping</name>
      <anchorfile>classpurify_1_1IndexMapping.html</anchorfile>
      <anchor>a08f286e69b232701b9be656282141668</anchor>
      <arglist>(const ITER &amp;first, const ITER &amp;end, const t_int N, const t_int start=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>IndexMapping</name>
      <anchorfile>classpurify_1_1IndexMapping.html</anchorfile>
      <anchor>aba763e2e761a4b1c1e092f0cd863476b</anchor>
      <arglist>(const std::set&lt; t_int &gt; &amp;indices, const t_int N, const t_int start=0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>classpurify_1_1IndexMapping.html</anchorfile>
      <anchor>ad46f33d32242d0e81bd165c2b5ad52bf</anchor>
      <arglist>(Eigen::MatrixBase&lt; T0 &gt; const &amp;input, Eigen::MatrixBase&lt; T1 &gt; const &amp;output) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>adjoint</name>
      <anchorfile>classpurify_1_1IndexMapping.html</anchorfile>
      <anchor>a7673f7ca2bfa8c75486e5b31463ee968</anchor>
      <arglist>(Eigen::MatrixBase&lt; T0 &gt; const &amp;input, Eigen::MatrixBase&lt; T1 &gt; const &amp;output) const</arglist>
    </member>
    <member kind="function">
      <type>t_int</type>
      <name>rows</name>
      <anchorfile>classpurify_1_1IndexMapping.html</anchorfile>
      <anchor>a62d55335cb5e55fefd70de5ac4f77f4e</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>t_int</type>
      <name>cols</name>
      <anchorfile>classpurify_1_1IndexMapping.html</anchorfile>
      <anchor>a1496703ce37d11748bd6b6d115b7cdd8</anchor>
      <arglist>() const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>purify::casa::MeasurementSet</name>
    <filename>http://astro-informatics.github.io/purify/classpurify_1_1casa_1_1MeasurementSet.html</filename>
    <class kind="class">purify::casa::MeasurementSet::ChannelWrapper</class>
    <class kind="class">purify::casa::MeasurementSet::const_iterator</class>
    <member kind="typedef">
      <type>Eigen::Array&lt; t_real, 2, 1 &gt;</type>
      <name>Direction</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet.html</anchorfile>
      <anchor>ab42446436227d207bfe3dc9990913188</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MeasurementSet</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet.html</anchorfile>
      <anchor>a0097a89df24950b62a5069e2cee21197</anchor>
      <arglist>(std::string const filename)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MeasurementSet</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet.html</anchorfile>
      <anchor>ad80cb6820032db20cf6488c6bfd55112</anchor>
      <arglist>(MeasurementSet const &amp;c)</arglist>
    </member>
    <member kind="function">
      <type>std::string const  &amp;</type>
      <name>filename</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet.html</anchorfile>
      <anchor>a30e4791dfd5fc60bc33273642fda93c9</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>MeasurementSet &amp;</type>
      <name>filename</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet.html</anchorfile>
      <anchor>a18d5649b6f6ee6a6a0f6f2464e09a35b</anchor>
      <arglist>(std::string const &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>::casacore::Table const  &amp;</type>
      <name>table</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet.html</anchorfile>
      <anchor>a62ee30f2803db3061fa4467c1f709391</anchor>
      <arglist>(std::string const &amp;name=&quot;&quot;) const</arglist>
    </member>
    <member kind="function">
      <type>::casacore::ScalarColumn&lt; T &gt;</type>
      <name>scalar_column</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet.html</anchorfile>
      <anchor>a21f1ba4d6c81fd431785925f428c93b2</anchor>
      <arglist>(std::string const &amp;col, std::string const &amp;tabname=&quot;&quot;) const</arglist>
    </member>
    <member kind="function">
      <type>::casacore::ArrayColumn&lt; T &gt;</type>
      <name>array_column</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet.html</anchorfile>
      <anchor>a5499d989cba2942af0dd73dc9ff2ba99</anchor>
      <arglist>(std::string const &amp;col, std::string const &amp;tabname=&quot;&quot;) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clear</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet.html</anchorfile>
      <anchor>ace5d2f559edd16429bb4aa2c2abc83ae</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; T &gt;</type>
      <name>column</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet.html</anchorfile>
      <anchor>af31bb35e3f992090902fd7df759a9851</anchor>
      <arglist>(std::string const &amp;column, std::string const &amp;filter=&quot;&quot;) const</arglist>
    </member>
    <member kind="function">
      <type>std::size_t</type>
      <name>size</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet.html</anchorfile>
      <anchor>ae7cead02c2d35c64138de20d0811e214</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>const_iterator</type>
      <name>begin</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet.html</anchorfile>
      <anchor>ad2a0b4da6dcf7b4312e8bb059b4d2f9f</anchor>
      <arglist>(std::string const &amp;filter=&quot;&quot;) const</arglist>
    </member>
    <member kind="function">
      <type>const_iterator</type>
      <name>end</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet.html</anchorfile>
      <anchor>a3208a43dc081294ece6d817dd37d252b</anchor>
      <arglist>(std::string const &amp;filter=&quot;&quot;) const</arglist>
    </member>
    <member kind="function">
      <type>ChannelWrapper</type>
      <name>operator[]</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet.html</anchorfile>
      <anchor>a68e749e2a4e759651035e557ebc0e1e9</anchor>
      <arglist>(t_uint i) const</arglist>
    </member>
    <member kind="function">
      <type>ChannelWrapper</type>
      <name>operator[]</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet.html</anchorfile>
      <anchor>afd142e45c6376649ab1bf388de4e459b</anchor>
      <arglist>(std::tuple&lt; t_uint, std::string &gt; const &amp;i) const</arglist>
    </member>
    <member kind="function">
      <type>Direction</type>
      <name>direction</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet.html</anchorfile>
      <anchor>a7b1b7974bb110c47e3cb2c46d367e326</anchor>
      <arglist>(t_real tolerance=1e-8, std::string const &amp;filter=&quot;&quot;) const</arglist>
    </member>
    <member kind="function">
      <type>Direction::Scalar</type>
      <name>right_ascension</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet.html</anchorfile>
      <anchor>a2f9746e819adf58f43fa64aba6d4216f</anchor>
      <arglist>(t_real tolerance=1e-8, std::string const &amp;filter=&quot;&quot;) const</arglist>
    </member>
    <member kind="function">
      <type>Direction::Scalar</type>
      <name>declination</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet.html</anchorfile>
      <anchor>acc99ad834580499123f6174984ce2b8e</anchor>
      <arglist>(t_real tolerance=1e-8, std::string const &amp;filter=&quot;&quot;) const</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static std::string const</type>
      <name>default_filter</name>
      <anchorfile>classpurify_1_1casa_1_1MeasurementSet.html</anchorfile>
      <anchor>a894d32452f89fd9c5bd634cbd3f61d30</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>NullReporter</name>
    <filename>http://astro-informatics.github.io/purify/classNullReporter.html</filename>
    <member kind="function">
      <type></type>
      <name>NullReporter</name>
      <anchorfile>classNullReporter.html</anchorfile>
      <anchor>a362f1a223088b7db18a6caf7bf5a7362</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>ReportContext</name>
      <anchorfile>classNullReporter.html</anchorfile>
      <anchor>aca50ae7883c629e6bf9927ab495433a6</anchor>
      <arglist>(const Context &amp;)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>ReportRuns</name>
      <anchorfile>classNullReporter.html</anchorfile>
      <anchor>a75c920ce9ec46fbbdbdae0683c4591c4</anchor>
      <arglist>(const std::vector&lt; Run &gt; &amp;)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>Finalize</name>
      <anchorfile>classNullReporter.html</anchorfile>
      <anchor>a39c2e27e5376f904fd6a0b63ed5c3291</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PadmmFixture</name>
    <filename>http://astro-informatics.github.io/purify/classPadmmFixture.html</filename>
    <member kind="function">
      <type>void</type>
      <name>SetUp</name>
      <anchorfile>classPadmmFixture.html</anchorfile>
      <anchor>a39a2b63e7620af30f2895a320707745c</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>TearDown</name>
      <anchorfile>classPadmmFixture.html</anchorfile>
      <anchor>ae17a92ac0fd5a72922988f285433cf99</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_counter</name>
      <anchorfile>classPadmmFixture.html</anchorfile>
      <anchor>afb10dbdcd1fc5760e68691b835d1f6ba</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const sopt::wavelets::SARA</type>
      <name>m_sara</name>
      <anchorfile>classPadmmFixture.html</anchorfile>
      <anchor>a47a8ec3f3f87024df032db1475b6d37a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Image&lt; t_complex &gt;</type>
      <name>m_image</name>
      <anchorfile>classPadmmFixture.html</anchorfile>
      <anchor>aeab69f13ca3fc32c29b1596fe84af14b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_imsizex</name>
      <anchorfile>classPadmmFixture.html</anchorfile>
      <anchor>a0f52961263dbcd1805c408f6594882cb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_imsizey</name>
      <anchorfile>classPadmmFixture.html</anchorfile>
      <anchor>abe3f1cca04ec7013dfdc7306ddeb9981</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>utilities::vis_params</type>
      <name>m_uv_data</name>
      <anchorfile>classPadmmFixture.html</anchorfile>
      <anchor>adfc4866343b7dd9880ce467e9ec4f1a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_real</type>
      <name>m_epsilon</name>
      <anchorfile>classPadmmFixture.html</anchorfile>
      <anchor>a39f8f59d0461c400adb0440de5e18bbf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_kernel</name>
      <anchorfile>classPadmmFixture.html</anchorfile>
      <anchor>a88d44c06e14e80625ce0c204449a6912</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::shared_ptr&lt; sopt::LinearTransform&lt; Vector&lt; t_complex &gt; &gt; const  &gt;</type>
      <name>m_measurements_transform</name>
      <anchorfile>classPadmmFixture.html</anchorfile>
      <anchor>a9a334d5265a7bdee18d189ef05b19291</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_real</type>
      <name>m_gamma</name>
      <anchorfile>classPadmmFixture.html</anchorfile>
      <anchor>a9dca78100e20ee41bb91724159f735a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::shared_ptr&lt; sopt::algorithm::ImagingProximalADMM&lt; t_complex &gt; &gt;</type>
      <name>m_padmm</name>
      <anchorfile>classPadmmFixture.html</anchorfile>
      <anchor>a5726a6434b465c8dce2c2885db7c8ea5</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PadmmFixtureMPI</name>
    <filename>http://astro-informatics.github.io/purify/classPadmmFixtureMPI.html</filename>
    <member kind="function">
      <type>void</type>
      <name>SetUp</name>
      <anchorfile>classPadmmFixtureMPI.html</anchorfile>
      <anchor>a055ba5bddb99e276fb0dbc0598655c95</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>TearDown</name>
      <anchorfile>classPadmmFixtureMPI.html</anchorfile>
      <anchor>a8aec27c003802523fce809122aa83244</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="variable">
      <type>sopt::mpi::Communicator</type>
      <name>m_world</name>
      <anchorfile>classPadmmFixtureMPI.html</anchorfile>
      <anchor>ad0072aa8caca8636e4e17056d3f7f513</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const sopt::wavelets::SARA</type>
      <name>m_sara</name>
      <anchorfile>classPadmmFixtureMPI.html</anchorfile>
      <anchor>a21d164da9b487d81e91b60ee36bfc732</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Image&lt; t_complex &gt;</type>
      <name>m_image</name>
      <anchorfile>classPadmmFixtureMPI.html</anchorfile>
      <anchor>a6ce27ab78e0b7dbb779481e3a3c1c472</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_imsizex</name>
      <anchorfile>classPadmmFixtureMPI.html</anchorfile>
      <anchor>a1649a03d84108e4e3806b577219c4799</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_imsizey</name>
      <anchorfile>classPadmmFixtureMPI.html</anchorfile>
      <anchor>ab6e5f6610cbef8c3117e2531a5b3f644</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>utilities::vis_params</type>
      <name>m_uv_data</name>
      <anchorfile>classPadmmFixtureMPI.html</anchorfile>
      <anchor>a3b2e53d88a4375ed989f68d87797acd7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_real</type>
      <name>m_epsilon</name>
      <anchorfile>classPadmmFixtureMPI.html</anchorfile>
      <anchor>a6c48054251bf4422af9090b2cb4e4f18</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_kernel</name>
      <anchorfile>classPadmmFixtureMPI.html</anchorfile>
      <anchor>a48cdb5d494320a659cdeda052a30d83d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::shared_ptr&lt; sopt::LinearTransform&lt; Vector&lt; t_complex &gt; &gt; const  &gt;</type>
      <name>m_measurements1</name>
      <anchorfile>classPadmmFixtureMPI.html</anchorfile>
      <anchor>a9905e7b4ba06263db0fbafa65b07eee4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::shared_ptr&lt; sopt::LinearTransform&lt; Vector&lt; t_complex &gt; &gt; const  &gt;</type>
      <name>m_measurements3</name>
      <anchorfile>classPadmmFixtureMPI.html</anchorfile>
      <anchor>ab93dfd28cc653e635f5966d8eecf6150</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>TmpMS</name>
    <filename>http://astro-informatics.github.io/purify/classTmpMS.html</filename>
    <base>TmpPath</base>
    <member kind="function">
      <type></type>
      <name>TmpMS</name>
      <anchorfile>classTmpMS.html</anchorfile>
      <anchor>a09c4c1453b7806d75ea9265be2de7077</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>casa::MeasurementSet const  &amp;</type>
      <name>operator*</name>
      <anchorfile>classTmpMS.html</anchorfile>
      <anchor>ac320ac074bc08683e0eb6f8e29a6e514</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>casa::MeasurementSet &amp;</type>
      <name>operator*</name>
      <anchorfile>classTmpMS.html</anchorfile>
      <anchor>ac5f17c077ef235349b6c60f4c4c9056e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>casa::MeasurementSet const  *</type>
      <name>operator-&gt;</name>
      <anchorfile>classTmpMS.html</anchorfile>
      <anchor>ae522e888f2a0fd4c7828e69c6c02c04a</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>casa::MeasurementSet *</type>
      <name>operator-&gt;</name>
      <anchorfile>classTmpMS.html</anchorfile>
      <anchor>aa18b4cb49be0e87d04a24719476f1091</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>TmpPath</name>
    <filename>http://astro-informatics.github.io/purify/classTmpPath.html</filename>
    <member kind="function">
      <type></type>
      <name>TmpPath</name>
      <anchorfile>classTmpPath.html</anchorfile>
      <anchor>a4209998a9e22c73294d8e6a2cdcb26e2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~TmpPath</name>
      <anchorfile>classTmpPath.html</anchorfile>
      <anchor>a190b031d44bcd0d7445edf1f5fa0e3a0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>boost::filesystem::path const  &amp;</type>
      <name>path</name>
      <anchorfile>classTmpPath.html</anchorfile>
      <anchor>ad14673d5ef0f1e258bd8027bddb570a3</anchor>
      <arglist>() const</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>purify::utilities::vis_params</name>
    <filename>http://astro-informatics.github.io/purify/structpurify_1_1utilities_1_1vis__params.html</filename>
    <member kind="function">
      <type>vis_params</type>
      <name>segment</name>
      <anchorfile>structpurify_1_1utilities_1_1vis__params.html</anchorfile>
      <anchor>a5c2e344d73e3f40fbfd98ca88f536f9e</anchor>
      <arglist>(const t_uint pos, const t_uint length) const</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>vis_params</name>
      <anchorfile>structpurify_1_1utilities_1_1vis__params.html</anchorfile>
      <anchor>a4d699385d0d94db144ff06f1f2f78ce2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>vis_params</name>
      <anchorfile>structpurify_1_1utilities_1_1vis__params.html</anchorfile>
      <anchor>abe094c25743e6d75215b92faae48116e</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;u_, const Vector&lt; t_real &gt; &amp;v_, const Vector&lt; t_real &gt; &amp;w_, const Vector&lt; t_complex &gt; &amp;vis_, const Vector&lt; t_complex &gt; &amp;weights_, const vis_units units_=vis_units::lambda, const t_real &amp;ra_=0, const t_real &amp;dec_=0, const t_real &amp;average_frequency_=0)</arglist>
    </member>
    <member kind="function">
      <type>t_uint</type>
      <name>size</name>
      <anchorfile>structpurify_1_1utilities_1_1vis__params.html</anchorfile>
      <anchor>a71a2094900171da4aa7f0ee62deebd53</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="variable">
      <type>Vector&lt; t_real &gt;</type>
      <name>u</name>
      <anchorfile>structpurify_1_1utilities_1_1vis__params.html</anchorfile>
      <anchor>a561d75ad12718981faf6d846d7a6bd3c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Vector&lt; t_real &gt;</type>
      <name>v</name>
      <anchorfile>structpurify_1_1utilities_1_1vis__params.html</anchorfile>
      <anchor>a069d0ab91900a49f16d16eb41e1df15d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Vector&lt; t_real &gt;</type>
      <name>w</name>
      <anchorfile>structpurify_1_1utilities_1_1vis__params.html</anchorfile>
      <anchor>a65de7bf9749e252c2f419fd29c8d2ca2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Vector&lt; t_real &gt;</type>
      <name>time</name>
      <anchorfile>structpurify_1_1utilities_1_1vis__params.html</anchorfile>
      <anchor>a127ac539d520c8daca09c56729dfc27f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Vector&lt; t_uint &gt;</type>
      <name>baseline</name>
      <anchorfile>structpurify_1_1utilities_1_1vis__params.html</anchorfile>
      <anchor>a53243946c1fc5056521dfabf6a5b06b9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Vector&lt; t_real &gt;</type>
      <name>frequencies</name>
      <anchorfile>structpurify_1_1utilities_1_1vis__params.html</anchorfile>
      <anchor>aa920f4c833af49430111a79d2323ab1e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Vector&lt; t_complex &gt;</type>
      <name>vis</name>
      <anchorfile>structpurify_1_1utilities_1_1vis__params.html</anchorfile>
      <anchor>a00fec6b3490e6c98c4d0befb677bfb09</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Vector&lt; t_complex &gt;</type>
      <name>weights</name>
      <anchorfile>structpurify_1_1utilities_1_1vis__params.html</anchorfile>
      <anchor>a91df0d9033a35225f6ed93bc2e3ab253</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>vis_units</type>
      <name>units</name>
      <anchorfile>structpurify_1_1utilities_1_1vis__params.html</anchorfile>
      <anchor>aadff757058a3bc040f9ddfc78ad47b1c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_real</type>
      <name>ra</name>
      <anchorfile>structpurify_1_1utilities_1_1vis__params.html</anchorfile>
      <anchor>a1f99c60f1d0691ecbf12cff5c6b99056</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_real</type>
      <name>dec</name>
      <anchorfile>structpurify_1_1utilities_1_1vis__params.html</anchorfile>
      <anchor>ab0bf945f28efc826e081b4b8955aec73</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_real</type>
      <name>average_frequency</name>
      <anchorfile>structpurify_1_1utilities_1_1vis__params.html</anchorfile>
      <anchor>a5551626c86ef47969b6736a12dc55127</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_real</type>
      <name>phase_centre_x</name>
      <anchorfile>structpurify_1_1utilities_1_1vis__params.html</anchorfile>
      <anchor>a522da1713a6e177bbdf05a782c537c9c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_real</type>
      <name>phase_centre_y</name>
      <anchorfile>structpurify_1_1utilities_1_1vis__params.html</anchorfile>
      <anchor>a3393c96fbcecc9107cc11c04bee864d5</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>WaveletOperatorAdjointFixture</name>
    <filename>http://astro-informatics.github.io/purify/classWaveletOperatorAdjointFixture.html</filename>
    <member kind="function">
      <type>void</type>
      <name>SetUp</name>
      <anchorfile>classWaveletOperatorAdjointFixture.html</anchorfile>
      <anchor>a7b128f0387b40d0dbc12f32e6ecd4a25</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>TearDown</name>
      <anchorfile>classWaveletOperatorAdjointFixture.html</anchorfile>
      <anchor>a728d04102340c1e9285e6fbf6dae629a</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_counter</name>
      <anchorfile>classWaveletOperatorAdjointFixture.html</anchorfile>
      <anchor>a113773ad364a0108f2ca6dc15802432a</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>WaveletOperatorFixture</name>
    <filename>http://astro-informatics.github.io/purify/classWaveletOperatorFixture.html</filename>
    <member kind="function">
      <type>void</type>
      <name>SetUp</name>
      <anchorfile>classWaveletOperatorFixture.html</anchorfile>
      <anchor>a2483d80901a6b2355be22e390aa918c1</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>TearDown</name>
      <anchorfile>classWaveletOperatorFixture.html</anchorfile>
      <anchor>acf52fd00fe14319c2e79a6e90f4e9dc6</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_counter</name>
      <anchorfile>classWaveletOperatorFixture.html</anchorfile>
      <anchor>a5b2f047ab91f69c63f6656b50776a18a</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>WaveletOperatorMPIFixture</name>
    <filename>http://astro-informatics.github.io/purify/classWaveletOperatorMPIFixture.html</filename>
    <member kind="function">
      <type></type>
      <name>WaveletOperatorMPIFixture</name>
      <anchorfile>classWaveletOperatorMPIFixture.html</anchorfile>
      <anchor>a5ef096fb7db79c9240791d7af966e945</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>SetUp</name>
      <anchorfile>classWaveletOperatorMPIFixture.html</anchorfile>
      <anchor>a176b978b0f4d7c5248deb76e80f3c227</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>TearDown</name>
      <anchorfile>classWaveletOperatorMPIFixture.html</anchorfile>
      <anchor>a20b24e3b5fba15e3cc5f8aa21401ee29</anchor>
      <arglist>(const ::benchmark::State &amp;state)</arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_counter</name>
      <anchorfile>classWaveletOperatorMPIFixture.html</anchorfile>
      <anchor>a2766fea0d94a0b3133cb895514832ef6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>sopt::mpi::Communicator</type>
      <name>m_world</name>
      <anchorfile>classWaveletOperatorMPIFixture.html</anchorfile>
      <anchor>a4f0eb9ae62052ebfc7b1f65e606d7ade</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>sopt::LinearTransform&lt; Vector&lt; t_complex &gt; &gt;</type>
      <name>m_Psi</name>
      <anchorfile>classWaveletOperatorMPIFixture.html</anchorfile>
      <anchor>a1e155cc96c7526a493b1eaf5a19dd2d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_imsizex</name>
      <anchorfile>classWaveletOperatorMPIFixture.html</anchorfile>
      <anchor>a65f4d1f0a5f0111d5d3e539730fd3b23</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>m_imsizey</name>
      <anchorfile>classWaveletOperatorMPIFixture.html</anchorfile>
      <anchor>ad7c60d26c7822945055ab08941d9c256</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>t_uint</type>
      <name>n_wave_coeff</name>
      <anchorfile>classWaveletOperatorMPIFixture.html</anchorfile>
      <anchor>a2323fc20fa322981cb18ce38cb8132bb</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>purify::YamlParser</name>
    <filename>http://astro-informatics.github.io/purify/classpurify_1_1YamlParser.html</filename>
    <member kind="function">
      <type></type>
      <name>YamlParser</name>
      <anchorfile>classpurify_1_1YamlParser.html</anchorfile>
      <anchor>af6cde51ba2666c4e94359e698ccf709c</anchor>
      <arglist>(const std::string &amp;filepath)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>readFile</name>
      <anchorfile>classpurify_1_1YamlParser.html</anchorfile>
      <anchor>a9cc8f622f6426637fed4f5adcee68383</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setParserVariablesFromYaml</name>
      <anchorfile>classpurify_1_1YamlParser.html</anchorfile>
      <anchor>ac81a876d2aa2c88947c6196a58a4d4ca</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>parseAndSetGeneralConfiguration</name>
      <anchorfile>classpurify_1_1YamlParser.html</anchorfile>
      <anchor>a643189d6794bb074f7a75c91ddaf9d05</anchor>
      <arglist>(const YAML::Node &amp;node)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>parseAndSetMeasureOperators</name>
      <anchorfile>classpurify_1_1YamlParser.html</anchorfile>
      <anchor>a010d338f823e5d2bcb49805e4cd11ca2</anchor>
      <arglist>(const YAML::Node &amp;node)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>parseAndSetSARA</name>
      <anchorfile>classpurify_1_1YamlParser.html</anchorfile>
      <anchor>a167e6e8fae445fe6069f0c3ab2ed846b</anchor>
      <arglist>(const YAML::Node &amp;node)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::string &gt;</type>
      <name>getWavelets</name>
      <anchorfile>classpurify_1_1YamlParser.html</anchorfile>
      <anchor>aecb769b470cf7ccf7d08f56d98e4810d</anchor>
      <arglist>(const std::string &amp;values_str)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>parseAndSetAlgorithmOptions</name>
      <anchorfile>classpurify_1_1YamlParser.html</anchorfile>
      <anchor>a660692be951d62f30a553193757b288b</anchor>
      <arglist>(const YAML::Node &amp;node)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>writeOutput</name>
      <anchorfile>classpurify_1_1YamlParser.html</anchorfile>
      <anchor>ae219f45210f4414e86e53ebd0176c0bc</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>b_utilities</name>
    <filename>http://astro-informatics.github.io/purify/namespaceb__utilities.html</filename>
    <member kind="function">
      <type>void</type>
      <name>Arguments</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>ab66b84f597f29c434e1666cf4c6f2668</anchor>
      <arglist>(benchmark::internal::Benchmark *b)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>duration</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>ac2c5b1baa8b1154965c0acbe63be33e1</anchor>
      <arglist>(std::chrono::high_resolution_clock::time_point start, std::chrono::high_resolution_clock::time_point end)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>updateImage</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>a8eea3bcb712683446eb05195bda70807</anchor>
      <arglist>(t_uint newSize, Image&lt; t_complex &gt; &amp;image, t_uint &amp;sizex, t_uint &amp;sizey)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>updateEmptyImage</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>aaf7e8fb274c41db4539e54be235ea03a</anchor>
      <arglist>(t_uint newSize, Vector&lt; t_complex &gt; &amp;image, t_uint &amp;sizex, t_uint &amp;sizey)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>updateMeasurements</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>a3298d69bf6039b98f16690284d9689c8</anchor>
      <arglist>(t_uint newSize, utilities::vis_params &amp;data)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>updateMeasurements</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>ab969056ca10702dbc6438b85f9fd583f</anchor>
      <arglist>(t_uint newSize, utilities::vis_params &amp;data, t_real &amp;epsilon, bool newImage, Image&lt; t_complex &gt; &amp;image)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; utilities::vis_params, t_real &gt;</type>
      <name>dirty_measurements</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>a92ba576f33170ef3a76ab53c758a2bed</anchor>
      <arglist>(Image&lt; t_complex &gt; const &amp;ground_truth_image, t_uint number_of_vis, t_real snr, const t_real &amp;cellsize)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>random_measurements</name>
      <anchorfile>namespaceb__utilities.html</anchorfile>
      <anchor>a7bf0bd5fea46254ea5c4e81af5c42d1c</anchor>
      <arglist>(t_int size)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>kernels_test</name>
    <filename>http://astro-informatics.github.io/purify/namespacekernels__test.html</filename>
    <member kind="variable">
      <type>const std::string</type>
      <name>test_dir</name>
      <anchorfile>namespacekernels__test.html</anchorfile>
      <anchor>afe7cd15fa169e6cab3e7a372d8ec882f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_real &gt;</type>
      <name>kernel_x_values</name>
      <anchorfile>namespacekernels__test.html</anchorfile>
      <anchor>abfe98f98ba03415a8e5d0dab22daf9b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_real &gt;</type>
      <name>kernel_pswf_values</name>
      <anchorfile>namespacekernels__test.html</anchorfile>
      <anchor>a7bb2fa229d396344b77fb114572058be</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_real &gt;</type>
      <name>kernel_kb_values</name>
      <anchorfile>namespacekernels__test.html</anchorfile>
      <anchor>abd51c1413c5c3da8244ec2aafa8c783f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_real &gt;</type>
      <name>kernel_gauss_values</name>
      <anchorfile>namespacekernels__test.html</anchorfile>
      <anchor>a0f906705aecd45568868ad81da26718c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_real &gt;</type>
      <name>correction_x_values</name>
      <anchorfile>namespacekernels__test.html</anchorfile>
      <anchor>ab5287a8320808f65ad59df31c593fa94</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_real &gt;</type>
      <name>correction_pswf_values</name>
      <anchorfile>namespacekernels__test.html</anchorfile>
      <anchor>a5ee213c2abf84def1dcf73447babb4b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_real &gt;</type>
      <name>correction_kb_values</name>
      <anchorfile>namespacekernels__test.html</anchorfile>
      <anchor>aeb51f1ab3a64e893a51328e52b874a9e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_real &gt;</type>
      <name>correction_gauss_values</name>
      <anchorfile>namespacekernels__test.html</anchorfile>
      <anchor>a8c014762dbb7842cd038bb3c29a9178c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>operators_test</name>
    <filename>http://astro-informatics.github.io/purify/namespaceoperators__test.html</filename>
    <member kind="variable">
      <type>const std::string</type>
      <name>test_dir</name>
      <anchorfile>namespaceoperators__test.html</anchorfile>
      <anchor>a165b68aac2b309ea97f327ee188141d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_real &gt;</type>
      <name>u</name>
      <anchorfile>namespaceoperators__test.html</anchorfile>
      <anchor>a694d86dbaf57f62a716d96eb8ce87bd5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_real &gt;</type>
      <name>v</name>
      <anchorfile>namespaceoperators__test.html</anchorfile>
      <anchor>a860ca9fd403e2b071edd997207df63ef</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_complex &gt;</type>
      <name>direct_input</name>
      <anchorfile>namespaceoperators__test.html</anchorfile>
      <anchor>aaad01d24d1b8fb40525dcc0504d71408</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_complex &gt;</type>
      <name>expected_direct</name>
      <anchorfile>namespaceoperators__test.html</anchorfile>
      <anchor>a8adeb5e5a9b6c5533348aadfe8e56bde</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_complex &gt;</type>
      <name>indirect_input</name>
      <anchorfile>namespaceoperators__test.html</anchorfile>
      <anchor>a662ce3a3cab05aa16f244d683370e790</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_complex &gt;</type>
      <name>expected_indirect</name>
      <anchorfile>namespaceoperators__test.html</anchorfile>
      <anchor>a40745badd4fc499fa6c184d038e353fd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::vector&lt; t_complex &gt;</type>
      <name>expected_S</name>
      <anchorfile>namespaceoperators__test.html</anchorfile>
      <anchor>a8fc882fbaad8516caa74583448397b2d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify.html</filename>
    <namespace>purify::casa</namespace>
    <namespace>purify::constant</namespace>
    <namespace>purify::convol</namespace>
    <namespace>purify::details</namespace>
    <namespace>purify::distribute</namespace>
    <namespace>purify::factory</namespace>
    <namespace>purify::integration</namespace>
    <namespace>purify::kernels</namespace>
    <namespace>purify::logging</namespace>
    <namespace>purify::measurementoperator</namespace>
    <namespace>purify::notinstalled</namespace>
    <namespace>purify::operators</namespace>
    <namespace>purify::pfitsio</namespace>
    <namespace>purify::projection_kernels</namespace>
    <namespace>purify::read_measurements</namespace>
    <namespace>purify::utilities</namespace>
    <namespace>purify::widefield</namespace>
    <namespace>purify::wproj_utilities</namespace>
    <class kind="class">purify::IndexMapping</class>
    <class kind="class">purify::YamlParser</class>
    <member kind="typedef">
      <type>std::complex&lt; float &gt;</type>
      <name>t_complexf</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a426969d60456233432af96386b69e341</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>Eigen::Triplet&lt; t_complex &gt;</type>
      <name>t_tripletList</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a854f899505b10ffd6c555674141c65cd</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>Eigen::SparseMatrix&lt; T, Eigen::RowMajor &gt;</type>
      <name>Sparse</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a9c5d741f10029a85f2a8e51fda9a4e07</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>Eigen::SparseVector&lt; T &gt;</type>
      <name>SparseVector</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a75c895e7cb33354e6559085c4cf5571d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>stokes</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a97d23bf901331999e4ef365e403ba8b8</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8add7536794b63bf90eccfd37f9b147d7f">I</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8af09564c9ca56850d4cd6b3319e541aee">Q</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8a4c614360da93c0a041b22e537de151eb">U</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8a5206560a306a2e085a437fd258eb57ce">V</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8ac51b57a703ba1c5869228690c93e1701">XX</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8a0867f43e27585e019c13f7f4b7c4ab6b">YY</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8a74c53bcd3dcb2bb79993b2fec37d362a">XY</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8a923db922542fbe09e7ff87bb31b2f310">YX</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8a67824ecf84f5816f07b74fa956bdbcd2">LL</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8acb95449a94688af33f6e9bb090cf2936">RR</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8a90a7c45eaffbd575ca6fb361e6d170a4">LR</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8a7f49bbe2f0af1edb6c6cee353d3e204b">RL</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="a97d23bf901331999e4ef365e403ba8b8a44c29edb103a2872f519ad0c9a0fdaaa">P</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>dde_type</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>ad1900deb6bebc3b75891904d6f576df5</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="ad1900deb6bebc3b75891904d6f576df5a15a19970dec9bb028b96daf2bb89af90">wkernel_radial</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify.html" anchor="ad1900deb6bebc3b75891904d6f576df5aa06fc4a5c57b80607e8a5d5a577bc796">wkernel_2d</enumvalue>
    </member>
    <member kind="function">
      <type>std::vector&lt; t_int &gt;</type>
      <name>all_to_all_recv_sizes</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a85ac6ed0c897c6398e3a7d09c966238a</anchor>
      <arglist>(const std::vector&lt; t_int &gt; &amp;local_indices, const t_int nodes, const t_int N)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; t_int &gt;</type>
      <name>all_to_all_send_sizes</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>ad40efb8e8768a0bf074af57e89d2e64c</anchor>
      <arglist>(const std::vector&lt; t_int &gt; &amp;recv_sizes, const sopt::mpi::Communicator &amp;comm)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>version</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a350eb23d7ebcdb7aae407717a7aad9d0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; uint8_t, uint8_t, uint8_t &gt;</type>
      <name>version_tuple</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>ac8f671077e81035877faca4f9bc5f25a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>gitref</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a5ccb5e37cec2a6be6fd26b42b0a66d79</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>default_logging_level</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a0108c76ec3e4a548088e9f5d5f6b4217</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>default_logger_name</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a4d58563620e972c279f841f66a8f68ef</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>constexpr bool</type>
      <name>color_logger</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a1719d0241f5cef19842ac9eb91272777</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::set&lt; t_int &gt;</type>
      <name>non_empty_outers</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>af9469839053b3fb67fdc9d0df0c35eba</anchor>
      <arglist>(Eigen::SparseMatrixBase&lt; T0 &gt; const &amp;matrix)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; typename T0::Scalar &gt;</type>
      <name>compress_outer</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>ae5a8e573961b3d00cc5603cf4e5e647f</anchor>
      <arglist>(T0 const &amp;matrix)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; std::function&lt; t_real(t_real)&gt;, std::function&lt; t_real(t_real)&gt;, std::function&lt; t_real(t_real)&gt;, std::function&lt; t_real(t_real)&gt; &gt;</type>
      <name>create_kernels</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a667d00cb4a0af96a8904a97f83797e7e</anchor>
      <arglist>(const kernels::kernel kernel_name_, const t_uint Ju_, const t_uint Jv_, const t_real imsizey_, const t_real imsizex_, const t_real oversample_ratio)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; std::function&lt; t_real(t_real)&gt;, std::function&lt; t_real(t_real)&gt; &gt;</type>
      <name>create_radial_ftkernel</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a3842f9866b28e4e47a59f78222b89b5f</anchor>
      <arglist>(const kernels::kernel kernel_name_, const t_uint Ju_, const t_real oversample_ratio)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::string &gt;</type>
      <name>split</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a75b4261fcc6cbc6e4733d6af03e294e0</anchor>
      <arglist>(const std::string &amp;s, char delim)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mkdir_recursive</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a105e14bc03a66d05ce538e8fce85d78b</anchor>
      <arglist>(const std::string &amp;path)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mkdir_recursive</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>aa7644e567d476e9f6123218bd3e0088f</anchor>
      <arglist>(const T &amp;path)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>split</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a0a3cf9b99373268388f8c5dee3255da7</anchor>
      <arglist>(const std::string &amp;s, char delim, T result)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>get_vector</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a05b6872149a85881d90befe9d341da05</anchor>
      <arglist>(const YAML::Node &amp;node_map, const std::initializer_list&lt; const char *&gt; indicies)</arglist>
    </member>
    <member kind="variable">
      <type>const std::map&lt; stokes, t_int &gt;</type>
      <name>stokes_int</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a28dc6ce59ab78004234888df0d8ba64f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const std::map&lt; std::string, stokes &gt;</type>
      <name>stokes_string</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify.html</anchorfile>
      <anchor>a563072861363d38f71744c9e817705c3</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify::casa</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</filename>
    <namespace>purify::casa::details</namespace>
    <class kind="class">purify::casa::MeasurementSet</class>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_measurementset</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>ab58d3422a795bff8b688ce650dc0b513</anchor>
      <arglist>(std::string const &amp;filename, const stokes pol=stokes::I, const std::vector&lt; t_int &gt; &amp;channels=std::vector&lt; t_int &gt;(), std::string const &amp;filter=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_measurementset</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>a18ac7690d95877e32386a1cdf20d24cd</anchor>
      <arglist>(std::string const &amp;filename, const utilities::vis_params &amp;uv1, const stokes pol, const std::vector&lt; t_int &gt; &amp;channels, std::string const &amp;filter)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_measurementset</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>a5cb78b815300138060a78e2e69610b12</anchor>
      <arglist>(std::vector&lt; std::string &gt; const &amp;filename, const stokes pol=stokes::I, const std::vector&lt; t_int &gt; &amp;channels=std::vector&lt; t_int &gt;(), std::string const &amp;filter=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_measurementset</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>a5a2e476c113ea88c281eb84e82deceb4</anchor>
      <arglist>(MeasurementSet const &amp;ms_file, const stokes pol=stokes::I, const std::vector&lt; t_int &gt; &amp;channels=std::vector&lt; t_int &gt;(), std::string const &amp;filter=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; utilities::vis_params &gt;</type>
      <name>read_measurementset_channels</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>aec0779879c336f74d45308d2333a8089</anchor>
      <arglist>(std::string const &amp;filename, const stokes pol=stokes::I, const t_int &amp;channel_width=1, std::string const &amp;filter=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>average_frequency</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>a401de6500811d6b4d4f6481975f3482d</anchor>
      <arglist>(const purify::casa::MeasurementSet &amp;ms_file, std::string const &amp;filter, const std::vector&lt; t_int &gt; &amp;channels)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; T &gt;</type>
      <name>table_column</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>a70ba290dc048b03efada7314b8ba5884</anchor>
      <arglist>(::casacore::Table const &amp;table, std::string const &amp;column, std::string const &amp;filter=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>MeasurementSet::const_iterator</type>
      <name>operator+</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>a9fe7e9400d83c6b30f7c4ea09dd05be4</anchor>
      <arglist>(MeasurementSet::const_iterator::difference_type n, MeasurementSet::const_iterator const &amp;c)</arglist>
    </member>
    <member kind="function">
      <type>MeasurementSet::const_iterator</type>
      <name>operator-</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa.html</anchorfile>
      <anchor>a8b4b2cd370ccf6a6ce9c864c8d8292dd</anchor>
      <arglist>(MeasurementSet::const_iterator::difference_type n, MeasurementSet::const_iterator const &amp;c)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify::casa::details</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify_1_1casa_1_1details.html</filename>
    <member kind="function">
      <type>Matrix&lt; C &gt;</type>
      <name>get_taql_array</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa_1_1details.html</anchorfile>
      <anchor>ae9df719c75711a86c94bc0486daa0de7</anchor>
      <arglist>(::casacore::TaQLResult const &amp;taql)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; C &gt;</type>
      <name>get_taql_scalar</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa_1_1details.html</anchorfile>
      <anchor>a4a16e38e7197aa89f4fa9302892a10ef</anchor>
      <arglist>(::casacore::TaQLResult const &amp;taql)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; T &gt;</type>
      <name>table_column</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa_1_1details.html</anchorfile>
      <anchor>aa097b6543eaa0e67a512c8a2b7a46fa9</anchor>
      <arglist>(::casacore::Table const &amp;table, std::string const &amp;column, std::string const &amp;filter, std::integral_constant&lt; bool, true &gt; const &amp;)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; T &gt;</type>
      <name>table_column</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1casa_1_1details.html</anchorfile>
      <anchor>ae6985d978f24bb26ad114a6504b8eaa0</anchor>
      <arglist>(::casacore::Table const &amp;table, std::string const &amp;column, std::string const &amp;filter, std::integral_constant&lt; bool, false &gt; const &amp;)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify::constant</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify_1_1constant.html</filename>
    <member kind="variable">
      <type>const t_real</type>
      <name>pi</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1constant.html</anchorfile>
      <anchor>aa35bce2e736d0453c6005cbc06f1dbe7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const t_real</type>
      <name>c</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1constant.html</anchorfile>
      <anchor>a289e794ad6812bcd3de2fdcc68dfb01b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify::convol</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify_1_1convol.html</filename>
    <member kind="function">
      <type>Vector&lt; T &gt;</type>
      <name>zero_pad</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1convol.html</anchorfile>
      <anchor>a1e84eceae2f9c870a3e678f6bd6763f9</anchor>
      <arglist>(const Vector&lt; T &gt; &amp;input, const t_int padding)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; T &gt;</type>
      <name>zero_pad</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1convol.html</anchorfile>
      <anchor>a4ae340f99f2c6cc64b316f8394382fbc</anchor>
      <arglist>(const Matrix&lt; T &gt; &amp;input, const t_int paddingv, const t_int paddingu)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; T &gt;</type>
      <name>linear_convol_1d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1convol.html</anchorfile>
      <anchor>adf71b9fa393f32b36d0cbc545d831360</anchor>
      <arglist>(const Vector&lt; T &gt; &amp;kernelf, const Vector&lt; T &gt; &amp;kernelg)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; T &gt;</type>
      <name>linear_convol_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1convol.html</anchorfile>
      <anchor>a8243104c62ca603ba048c521ca99be92</anchor>
      <arglist>(const Vector&lt; T &gt; &amp;kernelfu, const Vector&lt; T &gt; &amp;kernelfv, const Matrix&lt; T &gt; &amp;kernelg)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; T &gt;</type>
      <name>linear_convol_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1convol.html</anchorfile>
      <anchor>a8d09d81178acd9e2b707396697b39c1a</anchor>
      <arglist>(const std::function&lt; T(t_int)&gt; &amp;kernelu, const std::function&lt; T(t_int)&gt; &amp;kernelv, const std::function&lt; T(t_int, t_int)&gt; &amp;kernelw, const t_uint &amp;Jfu, const t_uint &amp;Jfv, const t_uint &amp;Jgu, const t_uint &amp;Jgv)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify::details</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify_1_1details.html</filename>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>init_gridding_matrix_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1details.html</anchorfile>
      <anchor>a96036c7a586e56d378dbbb4dad742144</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;u, const Vector&lt; t_real &gt; &amp;v, const Vector&lt; t_complex &gt; &amp;weights, const t_uint &amp;imsizey_, const t_uint &amp;imsizex_, const t_real &amp;oversample_ratio, const std::function&lt; t_real(t_real)&gt; kernelu, const std::function&lt; t_real(t_real)&gt; kernelv, const t_uint Ju, const t_uint Jv)</arglist>
    </member>
    <member kind="function">
      <type>Image&lt; t_complex &gt;</type>
      <name>init_correction2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1details.html</anchorfile>
      <anchor>a288d7453cd72dededadfb96cfcad60b4</anchor>
      <arglist>(const t_real &amp;oversample_ratio, const t_uint &amp;imsizey_, const t_uint &amp;imsizex_, const std::function&lt; t_real(t_real)&gt; ftkernelu, const std::function&lt; t_real(t_real)&gt; ftkernelv, const t_real &amp;w_mean, const t_real &amp;cellx, const t_real &amp;celly)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>init_gridding_matrix_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1details.html</anchorfile>
      <anchor>aadad7ec0a0d88c5ddd10f28a27d1b4a5</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;u, const Vector&lt; t_real &gt; &amp;v, const Vector&lt; t_real &gt; &amp;w, const Vector&lt; t_complex &gt; &amp;weights, const t_uint imsizey_, const t_uint imsizex_, const t_real oversample_ratio, const std::function&lt; t_real(t_real)&gt; &amp;ftkerneluv, const std::function&lt; t_real(t_real)&gt; &amp;kerneluv, const t_uint Ju, const t_uint Jw, const t_real cellx, const t_real celly, const t_real abs_error, const t_real rel_error, const dde_type dde)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>init_gridding_matrix_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1details.html</anchorfile>
      <anchor>a196bc201d3a368eeeadafc2ca859368a</anchor>
      <arglist>(const Sparse&lt; T &gt; &amp;mixing_matrix, ARGS &amp;&amp;... args)</arglist>
    </member>
    <member kind="function">
      <type>Image&lt; t_complex &gt;</type>
      <name>init_correction_radial_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1details.html</anchorfile>
      <anchor>a8baba6d82e8930a1b2571c541a660669</anchor>
      <arglist>(const t_real oversample_ratio, const t_uint imsizey_, const t_uint imsizex_, const std::function&lt; t_real(t_real)&gt; &amp;ftkerneluv, const t_real w_mean, const t_real cellx, const t_real celly)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify::distribute</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>plan</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</anchorfile>
      <anchor>a0bf24e0d0fb613140687838e4546ab2a</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html" anchor="a0bf24e0d0fb613140687838e4546ab2aa334c4a4c42fdb79d7ebc3e73b517e6f8">none</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html" anchor="a0bf24e0d0fb613140687838e4546ab2aa465289687a70db7aa7217cc240c29f0f">equal</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html" anchor="a0bf24e0d0fb613140687838e4546ab2aa869836a44b607137a7e094cf8a308938">radial</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html" anchor="a0bf24e0d0fb613140687838e4546ab2aa40b63143a987ddcf3aa69fb2ac32a3bd">w_term</enumvalue>
    </member>
    <member kind="function">
      <type>std::vector&lt; t_int &gt;</type>
      <name>distribute_measurements</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</anchorfile>
      <anchor>ac92e70bdd1349f71c586e2ae418aacac</anchor>
      <arglist>(Vector&lt; t_real &gt; const &amp;u, Vector&lt; t_real &gt; const &amp;v, Vector&lt; t_real &gt; const &amp;w, t_int const number_of_nodes, distribute::plan const distribution_plan=plan::equal, t_int const &amp;grid_size=128)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_int &gt;</type>
      <name>w_distribution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</anchorfile>
      <anchor>a4d4883a51a1bcc624ea4c2140e4b322d</anchor>
      <arglist>(Vector&lt; t_real &gt; const &amp;u, const Vector&lt; t_real &gt; &amp;v, const Vector&lt; t_real &gt; &amp;w)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_int &gt;</type>
      <name>w_distribution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</anchorfile>
      <anchor>aa84ba162545cce84263919102c2ada05</anchor>
      <arglist>(Vector&lt; t_real &gt; const &amp;w)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_int &gt;</type>
      <name>distance_distribution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</anchorfile>
      <anchor>afa3ab873e06dd32e3f2ce5e968900b93</anchor>
      <arglist>(Vector&lt; t_real &gt; const &amp;u, Vector&lt; t_real &gt; const &amp;v)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_int &gt;</type>
      <name>equal_distribution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</anchorfile>
      <anchor>a7c0aef9bb156212fd92deaaa148510fc</anchor>
      <arglist>(Vector&lt; t_real &gt; const &amp;u, Vector&lt; t_real &gt; const &amp;v, t_int const &amp;grid_size)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; std::vector&lt; t_int &gt;, std::vector&lt; t_real &gt; &gt;</type>
      <name>kmeans_algo</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1distribute.html</anchorfile>
      <anchor>ae6155f9cc952e699bb3b40941a166171</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;w, const t_int number_of_nodes, const t_int iters, const std::function&lt; t_real(t_real)&gt; &amp;cost=[](t_real x) { return x *x;})</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify::factory</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>algorithm</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>a61804e4c4f79fa94c6b864535459d5f8</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a61804e4c4f79fa94c6b864535459d5f8af6ff4586be2e5d12bbd6447df9719d9b">padmm</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a61804e4c4f79fa94c6b864535459d5f8a5de8b4afa495633c7ddf39c69642d187">primal_dual</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a61804e4c4f79fa94c6b864535459d5f8a610b120e176bdf628308e7f0f4ab3b7b">sdmm</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a61804e4c4f79fa94c6b864535459d5f8a9fd3baacb5ca6344ddb0ab9f5b71cb02">forward_backward</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>algo_distribution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>a862f0cad917b1ba718f965fd508f31f4</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a862f0cad917b1ba718f965fd508f31f4a74136b24217cb75599440e6e1d807cf7">serial</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a862f0cad917b1ba718f965fd508f31f4ac0fed08bcb99b1cd719f49ff80e9a912">mpi_serial</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a862f0cad917b1ba718f965fd508f31f4afd592d6cd0b4eadd3f6fa268b995ff76">mpi_distributed</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>ConvergenceType</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>ae3112c9c2dd6bbf06b1c59b4549f89c6</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="ae3112c9c2dd6bbf06b1c59b4549f89c6aec43da7d4be20024750a9c32d3236a9c">mpi_local</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="ae3112c9c2dd6bbf06b1c59b4549f89c6a23ddf3dd46824331a36d2d2b62e903fe">mpi_global</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>distributed_measurement_operator</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>a6b8de5116c3d7d0d679cd6dd47a80248</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a6b8de5116c3d7d0d679cd6dd47a80248a74136b24217cb75599440e6e1d807cf7">serial</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a6b8de5116c3d7d0d679cd6dd47a80248a3c25e5e5347476170daf56c780247dab">mpi_distribute_image</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a6b8de5116c3d7d0d679cd6dd47a80248ade789d500028594f8485e34f4fd201da">mpi_distribute_grid</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a6b8de5116c3d7d0d679cd6dd47a80248aeb435348cd1dd3b23bc83af306c8af4e">gpu_serial</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a6b8de5116c3d7d0d679cd6dd47a80248a3c5924386fd8ce3e082a2e4e2f73a3a2">gpu_mpi_distribute_image</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a6b8de5116c3d7d0d679cd6dd47a80248a91443239f9c1344dd49541bb99dfb3e0">gpu_mpi_distribute_grid</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>distributed_wavelet_operator</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>a5bbc6a01d0a2cdd30b656a6bb957221f</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a5bbc6a01d0a2cdd30b656a6bb957221fa74136b24217cb75599440e6e1d807cf7">serial</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html" anchor="a5bbc6a01d0a2cdd30b656a6bb957221fad560be68e475e7680c1ea7f87a40926e">mpi_sara</enumvalue>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; Algorithm &gt;</type>
      <name>algorithm_factory</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>aab5d94490e8d79a279fa050f697a13a1</anchor>
      <arglist>(const factory::algorithm algo, ARGS &amp;&amp;... args)</arglist>
    </member>
    <member kind="function">
      <type>std::enable_if&lt; std::is_same&lt; Algorithm, sopt::algorithm::ImagingProximalADMM&lt; t_complex &gt; &gt;::value, std::shared_ptr&lt; Algorithm &gt; &gt;::type</type>
      <name>padmm_factory</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>ad14f66077e408cf7ea6015983bed1383</anchor>
      <arglist>(const algo_distribution dist, std::shared_ptr&lt; sopt::LinearTransform&lt; Vector&lt; typename Algorithm::Scalar &gt;&gt; const &gt; const &amp;measurements, std::shared_ptr&lt; sopt::LinearTransform&lt; Vector&lt; typename Algorithm::Scalar &gt;&gt; const &gt; const &amp;wavelets, const utilities::vis_params &amp;uv_data, const t_real sigma, const t_uint imsizey, const t_uint imsizex, const t_uint sara_size, const t_uint max_iterations=500, const bool real_constraint=true, const bool positive_constraint=true, const bool tight_frame=false, const t_real relative_variation=1e-3, const t_real l1_proximal_tolerance=1e-2, const t_uint maximum_proximal_iterations=50)</arglist>
    </member>
    <member kind="function">
      <type>std::enable_if&lt; std::is_same&lt; Algorithm, sopt::algorithm::ImagingForwardBackward&lt; t_complex &gt; &gt;::value, std::shared_ptr&lt; Algorithm &gt; &gt;::type</type>
      <name>fb_factory</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>aa6f2e06bcd3586e4583b44f1feb07abd</anchor>
      <arglist>(const algo_distribution dist, std::shared_ptr&lt; sopt::LinearTransform&lt; Vector&lt; typename Algorithm::Scalar &gt;&gt; const &gt; const &amp;measurements, std::shared_ptr&lt; sopt::LinearTransform&lt; Vector&lt; typename Algorithm::Scalar &gt;&gt; const &gt; const &amp;wavelets, const utilities::vis_params &amp;uv_data, const t_real sigma, const t_real step_size, const t_real reg_parameter, const t_uint imsizey, const t_uint imsizex, const t_uint sara_size, const t_uint max_iterations=500, const bool real_constraint=true, const bool positive_constraint=true, const bool tight_frame=false, const t_real relative_variation=1e-3, const t_real l1_proximal_tolerance=1e-2, const t_uint maximum_proximal_iterations=50)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; sopt::LinearTransform&lt; T &gt; &gt;</type>
      <name>measurement_operator_factory</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>acb403f80232cc471d467fe92d5802c2d</anchor>
      <arglist>(const distributed_measurement_operator distribute, ARGS &amp;&amp;... args)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_updater</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>af6b310e472e594a788004536bb28d09b</anchor>
      <arglist>(std::weak_ptr&lt; Algo &gt; const algo_weak, const t_real step_size_scale, const t_real update_tol, const t_uint update_iters, const pfitsio::header_params &amp;update_solution_header, const pfitsio::header_params &amp;update_residual_header, const t_uint imsizey, const t_uint imsizex, const t_uint sara_size, const bool using_mpi)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; sopt::LinearTransform&lt; T &gt; const  &gt;</type>
      <name>wavelet_operator_factory</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>a2ffd3209dbb03d7390df879572cea326</anchor>
      <arglist>(const distributed_wavelet_operator distribute, const std::vector&lt; std::tuple&lt; std::string, t_uint &gt;&gt; &amp;wavelets, const t_uint imsizey, const t_uint imsizex, t_uint &amp;sara_size)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; sopt::LinearTransform&lt; T &gt; const  &gt;</type>
      <name>wavelet_operator_factory</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>a5c435fa169cac5a8135922b5558f3ae6</anchor>
      <arglist>(const distributed_wavelet_operator distribute, const std::vector&lt; std::tuple&lt; std::string, t_uint &gt;&gt; &amp;wavelets, const t_uint imsizey, const t_uint imsizex)</arglist>
    </member>
    <member kind="variable">
      <type>const std::map&lt; std::string, algo_distribution &gt;</type>
      <name>algo_distribution_string</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1factory.html</anchorfile>
      <anchor>a27d858e0728daf0eff9998983c8c72a5</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify::integration</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>norm_type</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>a5d8cd1673befa0bfd8cc7618ff5a9816</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html" anchor="a5d8cd1673befa0bfd8cc7618ff5a9816a23b79ae0fc0f07a3669598dd23c694cc">individual</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html" anchor="a5d8cd1673befa0bfd8cc7618ff5a9816ab738418ba05b7e56726509f09f725672">paired</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html" anchor="a5d8cd1673befa0bfd8cc7618ff5a9816a377fd569971eedeba8fbea28434a390a">l1</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html" anchor="a5d8cd1673befa0bfd8cc7618ff5a9816abec25675775e9e0a0d783a5018b463e3">l2</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html" anchor="a5d8cd1673befa0bfd8cc7618ff5a9816a707f3c5e9c59c9e5f347d2cbec59c66b">linf</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>method</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>a29ad3e35748f2774b6fe751c8f9a4cc9</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html" anchor="a29ad3e35748f2774b6fe751c8f9a4cc9a2510c39011c5be704182423e3a695e91">h</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html" anchor="a29ad3e35748f2774b6fe751c8f9a4cc9a83878c91171338902e0fe0fb97a8c47a">p</enumvalue>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>integrate</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>ae71083362428137f328d85469be8c255</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;xmin, const Vector&lt; t_real &gt; &amp;xmax, const std::function&lt; t_real(Vector&lt; t_real &gt;)&gt; &amp;func, const norm_type norm, const t_real required_abs_error, const t_real required_rel_error, const t_uint max_evaluations, const method methodtype)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>integrate</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>a10cce957449a468b764cf02c240848ac</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;xmin, const Vector&lt; t_real &gt; &amp;xmax, const std::function&lt; t_complex(Vector&lt; t_real &gt;)&gt; &amp;func, const norm_type norm, const t_real required_abs_error, const t_real required_rel_error, const t_uint max_evaluations, const method methodtype)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_real &gt;</type>
      <name>integrate</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>a7d5e16597c855bdf29d195364c353e18</anchor>
      <arglist>(const t_uint fdim, const Vector&lt; t_real &gt; &amp;xmin, const Vector&lt; t_real &gt; &amp;xmax, const std::function&lt; Vector&lt; t_real &gt;(Vector&lt; t_real &gt;)&gt; &amp;func, const norm_type norm, const t_real required_abs_error, const t_real required_rel_error, const t_uint max_evaluations, const method methodtype)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_complex &gt;</type>
      <name>integrate_v</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>a1cd627283e9e7bca5d0b763a9f7b05c5</anchor>
      <arglist>(const t_uint fdim, const t_uint npts, const Vector&lt; t_real &gt; &amp;xmin, const Vector&lt; t_real &gt; &amp;xmax, const std::vector&lt; std::function&lt; t_complex(Vector&lt; t_real &gt;)&gt;&gt; &amp;func, const norm_type norm, const t_real required_abs_error, const t_real required_rel_error, const t_uint max_evaluations, const method methodtype)</arglist>
    </member>
    <member kind="function">
      <type>error_norm</type>
      <name>norm_error</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>ad77278105245f570120de72b95222a08</anchor>
      <arglist>(norm_type norm)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>convolution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>a42f348e8892ab417a52e17adc46f549d</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;x0, const Vector&lt; t_real &gt; &amp;xmin, const Vector&lt; t_real &gt; &amp;xmax, const std::function&lt; t_complex(Vector&lt; t_real &gt;)&gt; &amp;func1, const std::function&lt; t_complex(Vector&lt; t_real &gt;)&gt; &amp;func2, const norm_type norm, const t_real required_abs_error, const t_real required_rel_error, const t_uint max_evaluations, const method methodtype)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_complex &gt;</type>
      <name>integrate_v</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>af0570a8b8956046214f724b5e6982344</anchor>
      <arglist>(const t_uint fdim, const Vector&lt; t_real &gt; &amp;xmin, const Vector&lt; t_real &gt; &amp;xmax, const std::vector&lt; std::function&lt; t_complex(Vector&lt; t_real &gt;)&gt;&gt; &amp;func, const norm_type norm, const t_real required_abs_error, const t_real required_rel_error, const t_uint max_evaluations, const method methodtype)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>convolution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1integration.html</anchorfile>
      <anchor>a89922f501faa29aeee1ddee469d7382c</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;x0, const std::function&lt; t_complex(Vector&lt; t_real &gt;)&gt; &amp;func1, const std::function&lt; t_complex(Vector&lt; t_real &gt;)&gt; &amp;func2, const norm_type norm, const t_real required_abs_error, const t_real required_rel_error, const t_uint max_evaluations, const method methodtype)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify::kernels</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>kernel</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a5d1a602d4c5e183ca2ea9bf831d61668</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html" anchor="a5d1a602d4c5e183ca2ea9bf831d61668aba34ea40525a4379add785228e37fe86">kb</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html" anchor="a5d1a602d4c5e183ca2ea9bf831d61668a710a4950250286365cf841f765a790f1">gauss</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html" anchor="a5d1a602d4c5e183ca2ea9bf831d61668a34be958a921e43d813a2075297d8e862">box</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html" anchor="a5d1a602d4c5e183ca2ea9bf831d61668a214d868f9c3f802c756980667a30cc65">pswf</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html" anchor="a5d1a602d4c5e183ca2ea9bf831d61668ac8f9b2014e544adacfc19dfeba3718e8">kbmin</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html" anchor="a5d1a602d4c5e183ca2ea9bf831d61668a575e9f12266a1af6d0b1cce8da8bc310">gauss_alt</enumvalue>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>kaiser_bessel</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a5ebd5a33dc46d12eca370fb011c6d20a</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>kaiser_bessel_general</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a95321ed82db7eb7e34a706bf0de8135a</anchor>
      <arglist>(const t_real x, const t_real J, const t_real alpha)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>ft_kaiser_bessel_general</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>afdc7151c592c299ad32241b9af5465e9</anchor>
      <arglist>(const t_real x, const t_real J, const t_real alpha)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>ft_kaiser_bessel</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a2703c3d44226204c45f3219cc890edef</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>gaussian</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>aca4681b624d0137ad6a594cf93393015</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>ft_gaussian</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>afd78fa880a5c40cc7d98d741efedea1d</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>calc_for_pswf</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a9ead940a0dfa9fcbc121c11a88e0b3dd</anchor>
      <arglist>(const t_real x, const t_real J, const t_real alpha)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>pswf</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>af392bdc1af64b376a17cf66e8a7f911a</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>ft_pswf</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a8c6fa490c8163e98471c720580b71c49</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_real &gt;</type>
      <name>kernel_samples</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a47eee0820c6469891800858427889494</anchor>
      <arglist>(const t_real total_samples, const std::function&lt; t_real(t_real)&gt; kernelu, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>kernel_linear_interp</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>add23bd0781527fd3fb7adfbdcd540c90</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;samples, const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>pill_box</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a7940b1b5055b0ada972997661383c30a</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>ft_pill_box</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>abe1d622ce6528a7ca83d509943503d22</anchor>
      <arglist>(const t_real x, const t_real J)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>gaussian_general</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a9144b80de7e992f02fa42d2f2e4557b7</anchor>
      <arglist>(const t_real x, const t_real J, const t_real sigma)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>ft_gaussian_general</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a539e03adfcf4e7bc963e8282a722940e</anchor>
      <arglist>(const t_real x, const t_real J, const t_real sigma)</arglist>
    </member>
    <member kind="variable">
      <type>const std::map&lt; std::string, kernel &gt;</type>
      <name>kernel_from_string</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1kernels.html</anchorfile>
      <anchor>a1e2f3c37e5c0c6e2f2a3282110891f31</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify::logging</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</filename>
    <member kind="function">
      <type>std::shared_ptr&lt; int &gt;</type>
      <name>initialize</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</anchorfile>
      <anchor>a766221e3a05f736017082766ede24d4a</anchor>
      <arglist>(std::string const &amp;name=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; int &gt;</type>
      <name>initialize</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</anchorfile>
      <anchor>a5a761be31c4fb31b3a9e636c17133904</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; int &gt;</type>
      <name>get</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</anchorfile>
      <anchor>a96517fef9cb15e2b3cff0235f12f4d0a</anchor>
      <arglist>(std::string const &amp;name=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; int &gt;</type>
      <name>get</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</anchorfile>
      <anchor>a02a987cbc0c3d7d4c26190b547053998</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_level</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</anchorfile>
      <anchor>a46c69647ca4c2d4fe100e50884d6b442</anchor>
      <arglist>(std::string const &amp;level, std::string const &amp;name)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_level</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</anchorfile>
      <anchor>a9bada47c3aae602e8e246a164d79bada</anchor>
      <arglist>(std::string const &amp;)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>has_level</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</anchorfile>
      <anchor>a793bccd5a5042f11480fedfcbdcb56b8</anchor>
      <arglist>(std::string const &amp;, std::string const &amp;)</arglist>
    </member>
    <member kind="variable">
      <type>const std::string</type>
      <name>name_prefix</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1logging.html</anchorfile>
      <anchor>ade846bfa55625b2ef77802f37adc2c86</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify::measurementoperator</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify_1_1measurementoperator.html</filename>
    <member kind="function">
      <type>std::shared_ptr&lt; sopt::LinearTransform&lt; T &gt; &gt;</type>
      <name>init_degrid_operator_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1measurementoperator.html</anchorfile>
      <anchor>aff5a8794ed28522484f6b2fdae3724cb</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;u, const Vector&lt; t_real &gt; &amp;v, const Vector&lt; t_real &gt; &amp;w, const Vector&lt; t_complex &gt; &amp;weights, const t_uint &amp;imsizey, const t_uint &amp;imsizex, const t_real &amp;oversample_ratio=2, const kernels::kernel kernel=kernels::kernel::kb, const t_uint Ju=4, const t_uint Jv=4, const bool w_stacking=false, const t_real &amp;cellx=1, const t_real &amp;celly=1)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; sopt::LinearTransform&lt; T &gt; &gt;</type>
      <name>init_degrid_operator_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1measurementoperator.html</anchorfile>
      <anchor>aa97759ff26b02066f150a1f83f447db6</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis_input, const t_uint &amp;imsizey, const t_uint &amp;imsizex, const t_real &amp;cell_x, const t_real &amp;cell_y, const t_real &amp;oversample_ratio=2, const kernels::kernel kernel=kernels::kernel::kb, const t_uint Ju=4, const t_uint Jv=4, const bool w_stacking=false)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; sopt::LinearTransform&lt; T &gt; &gt;</type>
      <name>init_degrid_operator_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1measurementoperator.html</anchorfile>
      <anchor>af2d03beb248bc90cbce87b61a676993c</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;u, const Vector&lt; t_real &gt; &amp;v, const Vector&lt; t_real &gt; &amp;w, const Vector&lt; t_complex &gt; &amp;weights, const t_uint imsizey, const t_uint imsizex, const t_real oversample_ratio, const kernels::kernel kernel, const t_uint Ju, const t_uint Jw, const bool w_stacking, const t_real cellx, const t_real celly, const t_real absolute_error, const t_real relative_error, const dde_type dde)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; sopt::LinearTransform&lt; T &gt; &gt;</type>
      <name>init_degrid_operator_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1measurementoperator.html</anchorfile>
      <anchor>af14d4bff532ed08e5f31dc24d0e1bde2</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis_input, const t_uint imsizey, const t_uint imsizex, const t_real cell_x, const t_real cell_y, const t_real oversample_ratio, const kernels::kernel kernel, const t_uint Ju, const t_uint Jw, const bool w_stacking, const t_real absolute_error, const t_real relative_error, const dde_type dde)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify::notinstalled</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</filename>
    <member kind="function">
      <type>std::enable_if&lt; std::is_scalar&lt; T &gt;::value, std::vector&lt; T &gt; &gt;::type</type>
      <name>read_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>af7cc59cd1e20fb709a98750d5acfc541</anchor>
      <arglist>(const std::string &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::enable_if&lt; std::is_same&lt; t_complex, T &gt;::value, std::vector&lt; T &gt; &gt;::type</type>
      <name>read_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>a6de72df1755781a71f9b5d0b26585f73</anchor>
      <arglist>(const std::string &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; t_complex &gt;</type>
      <name>read_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>a9fecf19b08db444e65ca342e7e599e53</anchor>
      <arglist>(const std::vector&lt; t_real &gt; &amp;input)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>data_directory</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>a2a9204ef1ce008b33ce31abbe59045bf</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>data_filename</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>a31500447173c424a79ff41f27ce64804</anchor>
      <arglist>(std::string const &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>scratch_directory</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>a39f480c23d66c09cc02801c33ea7dc28</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>scratch_filename</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>ad1628b70a7db77d2fee2aa5b1c79fbc8</anchor>
      <arglist>(std::string const &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>image_filename</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>adcd4a748b6eaf349d7a41cfc85428883</anchor>
      <arglist>(std::string const &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>visibility_filename</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>a0f5b36543cab9c5f594e8c3c8191f272</anchor>
      <arglist>(std::string const &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>vla_filename</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>ac34b6abbc10ba7a195b39343a407b941</anchor>
      <arglist>(std::string const &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>atca_filename</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>adf7cca24a1b2c8df495b8c2aadbc61f8</anchor>
      <arglist>(std::string const &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>gridding_filename</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>ac90b295d30fb2cee7bc89403dedc1615</anchor>
      <arglist>(std::string const &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>degridding_filename</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>a27f867ebed051d1c131c979ea8d5d4cc</anchor>
      <arglist>(std::string const &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>output_directory</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>a6e559eebbb8daf09c4a6a297982e9533</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>output_filename</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>a711465428b3f8363776594d7d2db98be</anchor>
      <arglist>(std::string const &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>ngc3256_ms</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1notinstalled.html</anchorfile>
      <anchor>adec66730b5b17d093151b170b6f954fc</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify::operators</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>fftw_plan</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</anchorfile>
      <anchor>a39f06b7cbec98e0ab8c7d05f643d5db0</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html" anchor="a39f06b7cbec98e0ab8c7d05f643d5db0a94e7e210543475d2dc1ee610e7b4af1d">estimate</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html" anchor="a39f06b7cbec98e0ab8c7d05f643d5db0a2925f77709f2db1f915deadc2940db88">measure</enumvalue>
    </member>
    <member kind="function">
      <type>std::tuple&lt; sopt::OperatorFunction&lt; T &gt;, sopt::OperatorFunction&lt; T &gt; &gt;</type>
      <name>init_gridding_matrix_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</anchorfile>
      <anchor>ac94ce709c2bbcf2f07d31a68fe962f82</anchor>
      <arglist>(ARGS &amp;&amp;... args)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; sopt::OperatorFunction&lt; T &gt;, sopt::OperatorFunction&lt; T &gt; &gt;</type>
      <name>init_zero_padding_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</anchorfile>
      <anchor>a1ece84ba389010359b10f07ee76bc984</anchor>
      <arglist>(const Image&lt; typename T::Scalar &gt; &amp;S, const t_real &amp;oversample_ratio)</arglist>
    </member>
    <member kind="function">
      <type>sopt::OperatorFunction&lt; T &gt;</type>
      <name>init_normalise</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</anchorfile>
      <anchor>acbb0f25b649b725caa29142a3c9acb43</anchor>
      <arglist>(const t_real &amp;op_norm)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; sopt::OperatorFunction&lt; T &gt;, sopt::OperatorFunction&lt; T &gt; &gt;</type>
      <name>init_weights_</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</anchorfile>
      <anchor>a1dc82eed77f88b50dd4c2689c88d53fe</anchor>
      <arglist>(const Vector&lt; t_complex &gt; &amp;weights)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; sopt::OperatorFunction&lt; T &gt;, sopt::OperatorFunction&lt; T &gt; &gt;</type>
      <name>init_FFT_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</anchorfile>
      <anchor>adb16231e49aafda70e2b117b4d6190fc</anchor>
      <arglist>(const t_uint &amp;imsizey_, const t_uint &amp;imsizex_, const t_real &amp;oversample_factor_, const fftw_plan fftw_plan_flag_=fftw_plan::measure)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; sopt::OperatorFunction&lt; T &gt;, sopt::OperatorFunction&lt; T &gt; &gt;</type>
      <name>base_padding_and_FFT_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</anchorfile>
      <anchor>ab5cfa89a68b63860b961d36535048b4b</anchor>
      <arglist>(const std::function&lt; t_real(t_real)&gt; &amp;ftkernelu, const std::function&lt; t_real(t_real)&gt; &amp;ftkernelv, const t_uint &amp;imsizey, const t_uint &amp;imsizex, const t_real &amp;oversample_ratio=2, const fftw_plan &amp;ft_plan=fftw_plan::measure, const t_real &amp;w_mean=0, const t_real &amp;cellx=1, const t_real &amp;celly=1)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; sopt::OperatorFunction&lt; T &gt;, sopt::OperatorFunction&lt; T &gt; &gt;</type>
      <name>base_degrid_operator_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</anchorfile>
      <anchor>a74b5c5a09a17518b70fb103a89444740</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;u, const Vector&lt; t_real &gt; &amp;v, const Vector&lt; t_real &gt; &amp;w, const Vector&lt; t_complex &gt; &amp;weights, const t_uint &amp;imsizey, const t_uint &amp;imsizex, const t_real &amp;oversample_ratio=2, const kernels::kernel kernel=kernels::kernel::kb, const t_uint Ju=4, const t_uint Jv=4, const fftw_plan &amp;ft_plan=fftw_plan::measure, const bool w_stacking=false, const t_real &amp;cellx=1, const t_real &amp;celly=1)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; sopt::OperatorFunction&lt; T &gt;, sopt::OperatorFunction&lt; T &gt; &gt;</type>
      <name>base_padding_and_FFT_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</anchorfile>
      <anchor>a5a9e812998fc6df0645d778f0201b7bd</anchor>
      <arglist>(const std::function&lt; t_real(t_real)&gt; &amp;ftkerneluv, const t_uint imsizey, const t_uint imsizex, const t_real oversample_ratio, const fftw_plan ft_plan, const t_real w_mean, const t_real cellx, const t_real celly)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; sopt::OperatorFunction&lt; T &gt;, sopt::OperatorFunction&lt; T &gt; &gt;</type>
      <name>base_degrid_operator_2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1operators.html</anchorfile>
      <anchor>a921459c522a44106315cf0a0500c43c0</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;u, const Vector&lt; t_real &gt; &amp;v, const Vector&lt; t_real &gt; &amp;w, const Vector&lt; t_complex &gt; &amp;weights, const t_uint &amp;imsizey, const t_uint &amp;imsizex, const t_real oversample_ratio, const kernels::kernel kernel, const t_uint Ju, const t_uint Jw, const fftw_plan ft_plan, const bool w_stacking, const t_real cellx, const t_real celly, const t_real absolute_error, const t_real relative_error, const dde_type dde)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify::pfitsio</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</filename>
    <class kind="struct">purify::pfitsio::header_params</class>
    <member kind="function">
      <type>void</type>
      <name>write_key</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a06beccf538c8dfa4ecc7826ed03ac6ff</anchor>
      <arglist>(fitsfile *fptr, const std::string &amp;key, const std::string &amp;value, const std::string &amp;comment, int *status)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write_key</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a0866652eb43862a9e7354b25d062810f</anchor>
      <arglist>(fitsfile *fptr, const std::string &amp;key, const char *value, const std::string &amp;comment, int *status)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write_history</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a8dd66c7d9d4daaa55b8d022606c1af65</anchor>
      <arglist>(fitsfile *fptr, const std::string &amp;context, const std::string &amp;history, int *status)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>read_key</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a431c4446cdb4d1b0ea4c2ebd6f4458fc</anchor>
      <arglist>(fitsfile *fptr, const std::string &amp;key, int *status)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a05de3285d7ec5d5a6e43d2e5761e8b12</anchor>
      <arglist>(const Image&lt; t_real &gt; &amp;eigen_image, const pfitsio::header_params &amp;header, const bool &amp;overwrite)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>aeeb2e8fe8e4e388c9846e497b4cb8382</anchor>
      <arglist>(const Image&lt; t_real &gt; &amp;image, const std::string &amp;fits_name, const std::string &amp;pix_units=&quot;Jy/Beam&quot;, const bool &amp;overwrite=true)</arglist>
    </member>
    <member kind="function">
      <type>Image&lt; t_complex &gt;</type>
      <name>read2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a2d7e29052f62225e3af9b8e8f1420fb4</anchor>
      <arglist>(const std::string &amp;fits_name)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write3d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a5cc9ae3bb5b792cc11b4be74088a7754</anchor>
      <arglist>(const std::vector&lt; Image&lt; t_real &gt;&gt; &amp;image, const pfitsio::header_params &amp;header, const bool &amp;overwrite=true)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write3d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>ab2064cf1926a00a2aeeeece32959c465</anchor>
      <arglist>(const std::vector&lt; Image&lt; t_real &gt;&gt; &amp;image, const std::string &amp;fits_name, const std::string &amp;pix_units=&quot;Jy/Beam&quot;, const bool &amp;overwrite=true)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Image&lt; t_complex &gt; &gt;</type>
      <name>read3d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a6899ef51fc550bbb6a8188f2f305b889</anchor>
      <arglist>(const std::string &amp;fits_name)</arglist>
    </member>
    <member kind="function">
      <type>std::enable_if&lt; std::is_scalar&lt; T &gt;::value, void &gt;::type</type>
      <name>write_history</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a585686ddb75d2e64e0c86c4efec3fdab</anchor>
      <arglist>(fitsfile *fptr, const std::string &amp;context, const T &amp;history, int *status)</arglist>
    </member>
    <member kind="function">
      <type>std::enable_if&lt; std::is_scalar&lt; T &gt;::value, void &gt;::type</type>
      <name>write_key</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a0a86095fdf622ce509cee700e48dba2e</anchor>
      <arglist>(fitsfile *fptr, const std::string &amp;key, const T &amp;value, const std::string &amp;comment, int *status)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a3e8e2c90dd18fd3343091b6e83213f60</anchor>
      <arglist>(const Eigen::EigenBase&lt; DERIVED &gt; &amp;input, int nx, int ny, const std::string &amp;fits_name, const std::string &amp;pix_units=&quot;Jy/Beam&quot;, const bool &amp;overwrite=true)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write2d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>abc28fee9e31953d95f3ddf065539a58c</anchor>
      <arglist>(const Eigen::EigenBase&lt; DERIVED &gt; &amp;input, int nx, int ny, const pfitsio::header_params &amp;header, const bool overwrite=true)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write3d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a3aa5fe65315f8b330c7111d9d48f1cca</anchor>
      <arglist>(const std::vector&lt; Eigen::EigenBase&lt; DERIVED &gt;&gt; &amp;input, int nx, int ny, const std::string &amp;fits_name, const std::string &amp;pix_units=&quot;Jy/Beam&quot;, const bool &amp;overwrite=true)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write3d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>af097e381fa7d660f76098f5666274e90</anchor>
      <arglist>(const std::vector&lt; Eigen::EigenBase&lt; DERIVED &gt;&gt; &amp;input, int nx, int ny, const pfitsio::header_params &amp;header, const bool &amp;overwrite=true)</arglist>
    </member>
    <member kind="function">
      <type>std::enable_if&lt; std::is_same&lt; t_real, typename T::Scalar &gt;::value, void &gt;::type</type>
      <name>write3d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a8850eb26ccec2d0956f8e27b11a15a04</anchor>
      <arglist>(const Eigen::EigenBase&lt; T &gt; &amp;image, const t_int rows, const t_int cols, const t_int chans, const std::string &amp;fits_name, const std::string &amp;pix_units=&quot;Jy/Beam&quot;, const bool &amp;overwrite=true)</arglist>
    </member>
    <member kind="function">
      <type>std::enable_if&lt; std::is_same&lt; t_real, typename T::Scalar &gt;::value, void &gt;::type</type>
      <name>write3d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>ac97fabc4b92258d4c306cfd486c253c0</anchor>
      <arglist>(const Eigen::EigenBase&lt; T &gt; &amp;image, const t_int rows, const t_int cols, const t_int chans, const pfitsio::header_params &amp;header, const bool &amp;overwrite)</arglist>
    </member>
    <member kind="function">
      <type>std::enable_if&lt; std::is_same&lt; t_real, typename T::Scalar &gt;::value, void &gt;::type</type>
      <name>read3d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a252309ca86301bfadce2323718d01178</anchor>
      <arglist>(const std::string &amp;fits_name, Eigen::EigenBase&lt; T &gt; &amp;output, t_int &amp;rows, t_int &amp;cols, t_int &amp;channels, t_int &amp;pols)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_uvfits</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>ab302818131b95815c67a68477508881d</anchor>
      <arglist>(const std::vector&lt; std::string &gt; &amp;names, const bool flag=true, const stokes pol=stokes::I)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_uvfits</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a0e659096cde08d8f8f2d5f49c9f6e99e</anchor>
      <arglist>(const std::string &amp;vis_name2, const utilities::vis_params &amp;u1, const bool flag=true, const stokes pol=stokes::I)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_uvfits</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a4c0c4a69590d4d6e31eb99261dab097f</anchor>
      <arglist>(const std::string &amp;filename, const bool flag=true, const stokes pol=stokes::I)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>filter_and_combine</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a0f3c5af553b929cdfb8547316063a326</anchor>
      <arglist>(const utilities::vis_params &amp;input, const utilities::vis_params &amp;input2, const Vector&lt; t_complex &gt; stokes_transform, const std::function&lt; bool(t_real, t_real, t_real, t_complex, t_complex, t_real, t_real, t_real, t_complex, t_complex)&gt; &amp;filter=[](const t_real, const t_real, const t_real, const t_complex vis1, const t_complex weight1, const t_real, const t_real, const t_real, const t_complex vis2, const t_complex weight2) { return(weight1.real() &gt; 0.) and(weight2.real() &gt; 0.) and(std::abs(vis1) &gt; 1e-20) and(std::abs(vis2) &gt; 1e-20) and(!std::isnan(vis1.real()) and !std::isnan(vis1.imag())) and(!std::isnan(vis2.real()) and !std::isnan(vis2.imag()));})</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_polarisation</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>ae96a909089a418869696283dd65731bc</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;data, const Matrix&lt; t_real &gt; &amp;coords, const Vector&lt; t_real &gt; &amp;frequencies, const t_uint pol_index1, const t_uint pols, const t_uint baselines, const t_uint channels)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_real &gt;</type>
      <name>read_uvfits_freq</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a2bb1a445fbcf5e31447e1c45e563c3ba</anchor>
      <arglist>(fitsfile *fptr, t_int *status, const t_int &amp;col)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>read_uvfits_freq</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>ae2722e4945a6e8d0e138508eac2c97c4</anchor>
      <arglist>(fitsfile *fptr, t_int *status, Vector&lt; t_real &gt; &amp;output, const t_int &amp;col)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_real &gt;</type>
      <name>read_uvfits_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>ad4ac6ff4de411bcc95d5371b6c7e57b3</anchor>
      <arglist>(fitsfile *fptr, t_int *status, const std::vector&lt; t_int &gt; &amp;naxis, const t_int &amp;baselines)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>read_uvfits_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>abcf70ac31a8fd099e7934bbe3e25302f</anchor>
      <arglist>(fitsfile *fptr, t_int *status, const std::vector&lt; t_int &gt; &amp;naxis, const t_int &amp;baselines, Vector&lt; t_real &gt; &amp;output)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_real &gt;</type>
      <name>read_uvfits_coords</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>ab379a780acc3377f4d94e34535edde48</anchor>
      <arglist>(fitsfile *fptr, t_int *status, const t_int &amp;groups, const t_int &amp;pcount)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>read_value_from_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a4ad9e69e80d82aaf5925b6da550e1d85</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;data, const t_uint col, const t_uint pol, const t_uint pols, const t_uint chan, const t_uint chans, const t_uint baseline, const t_uint baselines)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>read_vis_from_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>af7c72608a0a44fc31e640b6e77be7748</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;data, const t_uint pol, const t_uint pols, const t_uint chan, const t_uint chans, const t_uint baseline, const t_uint baselines)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>read_weight_from_data</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a5e34256158689b24229b60876e06d174</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;data, const t_uint pol, const t_uint pols, const t_uint chan, const t_uint chans, const t_uint baseline, const t_uint baselines)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>read_uvfits_coords</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a2aed6f63e56ae45b4fe2be1031b59d62</anchor>
      <arglist>(fitsfile *fptr, t_int *status, const t_int &amp;pcount, const t_int &amp;groups, Matrix&lt; t_real &gt; &amp;output)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>read_fits_keys</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1pfitsio.html</anchorfile>
      <anchor>a3a4c55cd64e7b4e8650e400a82b8fc40</anchor>
      <arglist>(fitsfile *fptr, t_int *status)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify::projection_kernels</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify_1_1projection__kernels.html</filename>
    <member kind="function">
      <type>t_complex</type>
      <name>fourier_wproj_kernel</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1projection__kernels.html</anchorfile>
      <anchor>abf118d404395c7699556217d0474950e</anchor>
      <arglist>(const t_real x, const t_real y, const t_real w, const t_real u, const t_real v, const t_real du, const t_real dv)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>hankel_wproj_kernel</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1projection__kernels.html</anchorfile>
      <anchor>aea4249664d112544ddb02235e8f972d2</anchor>
      <arglist>(const t_real r, const t_real w, const t_real u, const t_real v, const t_real du)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>exact_w_projection_integration_1d</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1projection__kernels.html</anchorfile>
      <anchor>a1268ba056206a35e13d2faad51da6990</anchor>
      <arglist>(const t_real u, const t_real v, const t_real w, const t_real du, const t_real oversample_ratio, const std::function&lt; t_complex(t_real)&gt; &amp;ftkerneluv, const t_uint &amp;max_evaluations, const t_real &amp;absolute_error, const t_real &amp;relative_error, const integration::method method, t_uint &amp;evaluations)</arglist>
    </member>
    <member kind="function">
      <type>t_complex</type>
      <name>exact_w_projection_integration</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1projection__kernels.html</anchorfile>
      <anchor>a3363eb7199ce493e3b828292c7ece53a</anchor>
      <arglist>(const t_real u, const t_real v, const t_real w, const t_real du, const t_real dv, const t_real oversample_ratio, const std::function&lt; t_complex(t_real)&gt; &amp;ftkernelu, const std::function&lt; t_complex(t_real)&gt; &amp;ftkernelv, const t_uint &amp;max_evaluations, const t_real &amp;absolute_error, const t_real &amp;relative_error, const integration::method method, t_uint &amp;evaluations)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify::read_measurements</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>format</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html</anchorfile>
      <anchor>acd8f0c1e37901dde05b3ca586809723c</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html" anchor="acd8f0c1e37901dde05b3ca586809723cac3e888ce42796ce1d5780a4ea8197f0a">vis</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html" anchor="acd8f0c1e37901dde05b3ca586809723cabb130d30d26131335c78557f26dafbcf">uvfits</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html" anchor="acd8f0c1e37901dde05b3ca586809723caee33e909372d935d190f4fcb2a92d542">ms</enumvalue>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_measurements</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html</anchorfile>
      <anchor>a84cbb7fbe2a5562c37ed918ab4f57f92</anchor>
      <arglist>(const std::string &amp;name, const bool w_term=false, const stokes pol=stokes::I, const utilities::vis_units units=utilities::vis_units::lambda)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_measurements</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html</anchorfile>
      <anchor>a27cfe930b0c597cc75a41b1dc0c4323c</anchor>
      <arglist>(const std::vector&lt; std::string &gt; &amp;names, const bool w_term=false, const stokes pol=stokes::I, const utilities::vis_units units=utilities::vis_units::lambda)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>file_exists</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html</anchorfile>
      <anchor>a58ca8b3a5ff4855f72be8729145d3f54</anchor>
      <arglist>(const std::string &amp;path)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>dir_exists</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1read__measurements.html</anchorfile>
      <anchor>a433571b90f8e372071cafeda9632180f</anchor>
      <arglist>(const std::string &amp;path)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify::utilities</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</filename>
    <class kind="struct">purify::utilities::vis_params</class>
    <member kind="enumeration">
      <type></type>
      <name>vis_source</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>af608b7e6a570ae7289e797c577850a64</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html" anchor="af608b7e6a570ae7289e797c577850a64ac37be74d84f98bd288693a6bbee7a334">measurements</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html" anchor="af608b7e6a570ae7289e797c577850a64a9e083ec666c9f3db044bb7c381640227">simulation</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>vis_units</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>aa2af2ee7e4f1bb93a42235b9d973983d</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html" anchor="aa2af2ee7e4f1bb93a42235b9d973983da945f3fc449518a73b9f5f32868db466c">lambda</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html" anchor="aa2af2ee7e4f1bb93a42235b9d973983da017969ba49117264eb077e9f66daacfd">radians</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html" anchor="aa2af2ee7e4f1bb93a42235b9d973983dad399848208da8b80a306af0fd62bb03f">pixels</enumvalue>
    </member>
    <member kind="function">
      <type>void</type>
      <name>regroup</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a4a87838cfac5f5f656a75e0cdd33693b</anchor>
      <arglist>(vis_params &amp;uv_params, std::vector&lt; t_int &gt; const &amp;groups_, const t_int max_groups)</arglist>
    </member>
    <member kind="function">
      <type>vis_params</type>
      <name>regroup_and_scatter</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>acd42c5df2679743eb61058c3be5eaf01</anchor>
      <arglist>(vis_params const &amp;params, std::vector&lt; t_int &gt; const &amp;groups, sopt::mpi::Communicator const &amp;comm)</arglist>
    </member>
    <member kind="function">
      <type>vis_params</type>
      <name>regroup_and_all_to_all</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ac2aafb0f04fe070f18ff3b4afe66a4d8</anchor>
      <arglist>(vis_params const &amp;params, std::vector&lt; t_int &gt; const &amp;groups, sopt::mpi::Communicator const &amp;comm)</arglist>
    </member>
    <member kind="function">
      <type>vis_params</type>
      <name>all_to_all_visibilities</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a8e83fa8e3018614508a2a88e689bd260</anchor>
      <arglist>(vis_params const &amp;params, std::vector&lt; t_int &gt; const &amp;sizes, sopt::mpi::Communicator const &amp;comm)</arglist>
    </member>
    <member kind="function">
      <type>vis_params</type>
      <name>scatter_visibilities</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>abdce09971a0be5d91b6db1f978865710</anchor>
      <arglist>(vis_params const &amp;params, std::vector&lt; t_int &gt; const &amp;sizes, sopt::mpi::Communicator const &amp;comm)</arglist>
    </member>
    <member kind="function">
      <type>vis_params</type>
      <name>scatter_visibilities</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a787800823820f9480ef13458b3496aa7</anchor>
      <arglist>(sopt::mpi::Communicator const &amp;comm)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>distribute_params</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a2120ca26a6a6f0797051d7701d96ad14</anchor>
      <arglist>(utilities::vis_params const &amp;params, sopt::mpi::Communicator const &amp;comm)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>set_cell_size</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ae46ed33e75ac4e9921e1dd9ac8f8f4f0</anchor>
      <arglist>(const sopt::mpi::Communicator &amp;comm, utilities::vis_params const &amp;uv_vis, const t_real &amp;cell_x, const t_real &amp;cell_y)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>w_stacking</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>af19b6e8e20f56ac162dbbb8ad8136c51</anchor>
      <arglist>(utilities::vis_params const &amp;params, sopt::mpi::Communicator const &amp;comm, const t_int iters, const std::function&lt; t_real(t_real)&gt; &amp;cost)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>step_size</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a49598f35180a53cbfef9c5d44dad7ec0</anchor>
      <arglist>(T const &amp;vis, const std::shared_ptr&lt; sopt::LinearTransform&lt; T &gt; const &gt; &amp;measurements, const std::shared_ptr&lt; sopt::LinearTransform&lt; T &gt; const &gt; &amp;wavelets, const t_uint sara_size)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_real &gt;</type>
      <name>generate_antennas</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a14f77d27cb3daf69f4d53a1bc551d6ec</anchor>
      <arglist>(const t_uint N)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>antenna_to_coverage</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a0a278d13ff1ec7afe890960a836fe8b8</anchor>
      <arglist>(const t_uint N)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>antenna_to_coverage</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>aa38e259a0d826280ef79d31dfb897a60</anchor>
      <arglist>(const Matrix&lt; t_real &gt; &amp;B)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>random_sample_density</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ae8446d8749e342037d66da0ad3ca63ad</anchor>
      <arglist>(const t_int vis_num, const t_real mean, const t_real standard_deviation, const t_real rms_w=0)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_visibility</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ad4492cfc4a58430ffa09a7c002db7988</anchor>
      <arglist>(const std::vector&lt; std::string &gt; &amp;names, const bool w_term=false)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_visibility</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>abbad783bede3e70d3d62520cac7cdc76</anchor>
      <arglist>(const std::string &amp;vis_name2, const utilities::vis_params &amp;u1)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>streamtoreal</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a796bee1239c7873850c229f57d637189</anchor>
      <arglist>(std::ifstream &amp;stream)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>read_visibility</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>aae90971eb2fdab205c39d54868ce66e5</anchor>
      <arglist>(const std::string &amp;vis_name, const bool w_term=false)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write_visibility</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a083e82d5b1fdc9cae30da55252492c0f</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis, const std::string &amp;file_name, const bool w_term=false)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>set_cell_size</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a9fe18dbda5ae6286e280b563977c0fae</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis, const t_real &amp;max_u, const t_real &amp;max_v, const t_real &amp;input_cell_size_u, const t_real &amp;input_cell_size_v)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>set_cell_size</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a97777e616ef68893b4b62677cf2286fe</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis, const t_real &amp;cell_size_u=0, const t_real &amp;cell_size_v=0)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>uv_scale</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a5bc6a589888c5f3cc468acbb0c3bee37</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis, const t_int &amp;ftsizeu, const t_int &amp;ftsizev)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>conjugate_w</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a8f267ff244270871a0d36b52ff9b3eb6</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis)</arglist>
    </member>
    <member kind="function">
      <type>t_int</type>
      <name>sub2ind</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a7d6e0055d484aac9b7728a6614a80a07</anchor>
      <arglist>(const t_int &amp;row, const t_int &amp;col, const t_int &amp;rows, const t_int &amp;cols)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; t_int, t_int &gt;</type>
      <name>ind2sub</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>afb213bdf3ecdb4d9557e810349443234</anchor>
      <arglist>(const t_int &amp;sub, const t_int &amp;cols, const t_int &amp;rows)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>mod</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ae7983367cb937c38c31b16bd1e4619fa</anchor>
      <arglist>(const t_real &amp;x, const t_real &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Image&lt; t_complex &gt;</type>
      <name>convolution_operator</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>af717a83833a9f2025be0d26ef8f08788</anchor>
      <arglist>(const Image&lt; t_complex &gt; &amp;a, const Image&lt; t_complex &gt; &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>utilities::vis_params</type>
      <name>whiten_vis</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ae894867ada4d1a22fb98025e8260cc71</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>calculate_l2_radius</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>af90182ef1213164564e72e51f8a83ac7</anchor>
      <arglist>(const t_uint y_size, const t_real &amp;sigma=0, const t_real &amp;n_sigma=2., const std::string distirbution=&quot;chi&quot;)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>SNR_to_standard_deviation</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a4b530b0918705248f2d2dd4de0bb29f6</anchor>
      <arglist>(const Vector&lt; t_complex &gt; &amp;y0, const t_real &amp;SNR=30.)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_complex &gt;</type>
      <name>add_noise</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a7788412bbecc48c81b8e5777943b5666</anchor>
      <arglist>(const Vector&lt; t_complex &gt; &amp;y, const t_complex &amp;mean, const t_real &amp;standard_deviation)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>file_exists</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>aa251252a271965c9568d3f3bd8e37259</anchor>
      <arglist>(const std::string &amp;name)</arglist>
    </member>
    <member kind="function">
      <type>Vector&lt; t_real &gt;</type>
      <name>fit_fwhm</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a9a837da30f295946510cbeccf9bf4abf</anchor>
      <arglist>(const Image&lt; t_real &gt; &amp;psf, const t_int &amp;size=3)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>median</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a1e331f75754f76388cfbf6e586f8d276</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;input)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>dynamic_range</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ad55d48f38b4f24dfc7d703682fb3e26d</anchor>
      <arglist>(const Image&lt; t_complex &gt; &amp;model, const Image&lt; t_complex &gt; &amp;residuals, const t_real &amp;operator_norm=1)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; t_complex &gt;</type>
      <name>init_weights</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a0bb5cc817f651a8bf2d9eebdfbf91caf</anchor>
      <arglist>(const Vector&lt; t_real &gt; &amp;u, const Vector&lt; t_real &gt; &amp;v, const Vector&lt; t_complex &gt; &amp;weights, const t_real &amp;oversample_factor, const std::string &amp;weighting_type, const t_real &amp;R, const t_int &amp;ftsizeu, const t_int &amp;ftsizev)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; t_int, t_real &gt;</type>
      <name>checkpoint_log</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a90ba5f2c558a7c02319096b2ad7f48b1</anchor>
      <arglist>(const std::string &amp;diagnostic)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>re_sample_ft_grid</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a99180a66ed0f610008564648b73057b5</anchor>
      <arglist>(const Matrix&lt; t_complex &gt; &amp;input, const t_real &amp;re_sample_factor)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>re_sample_image</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a9f6f39cdfdf8c9a8136f69ec144d2e47</anchor>
      <arglist>(const Matrix&lt; t_complex &gt; &amp;input, const t_real &amp;re_sample_ratio)</arglist>
    </member>
    <member kind="function">
      <type>K::Scalar</type>
      <name>mean</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a616adaec8411e901dfaf0e2728f1a9dc</anchor>
      <arglist>(const K x)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>variance</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a327605e8997b50446f40a76434764e88</anchor>
      <arglist>(const K x)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>standard_deviation</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>ae9a47127ad2c0c43f8387e0da2cf804b</anchor>
      <arglist>(const K x)</arglist>
    </member>
    <member kind="function">
      <type>std::enable_if&lt; std::is_same&lt; typename T0::Scalar, typename T1::Scalar &gt;::value and T0::IsRowMajor, Vector&lt; typename T0::Scalar &gt; &gt;::type</type>
      <name>sparse_multiply_matrix</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a0ff3f9b89549f7e5a46cf56524e4e7cc</anchor>
      <arglist>(const Eigen::SparseMatrixBase&lt; T0 &gt; &amp;M, const Eigen::MatrixBase&lt; T1 &gt; &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>Image&lt; t_complex &gt;</type>
      <name>parallel_multiply_image</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>afff87a1bc7664b5277e71cbf7f7f217f</anchor>
      <arglist>(const K &amp;A, const L &amp;B)</arglist>
    </member>
    <member kind="function">
      <type>vis_params</type>
      <name>sort_by_w</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>af19ddf9b8eb98a1298d163db8472b663</anchor>
      <arglist>(const vis_params &amp;uv_data)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>w_lambert</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a0a3636b5cc986bcc48ed8a437d4c1f06</anchor>
      <arglist>(T x, const t_real &amp;relative_difference)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>w_lambert</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1utilities.html</anchorfile>
      <anchor>a561048f21d1a1569b5d0d3123c1d45ed</anchor>
      <arglist>(const T &amp;x, const t_real &amp;relative_difference=1e-8)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify::widefield</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify_1_1widefield.html</filename>
    <member kind="function">
      <type>t_int</type>
      <name>w_support</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1widefield.html</anchorfile>
      <anchor>a72665d41e585afb454323ad5b3e2a5cc</anchor>
      <arglist>(const t_real w, const t_real du, const t_int min, const t_int max)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>pixel_to_lambda</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1widefield.html</anchorfile>
      <anchor>ab62e83c2e2504a0e5b0e0d333d311b52</anchor>
      <arglist>(const t_real cell, const t_uint imsize, const t_real oversample_ratio)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>estimate_cell_size</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1widefield.html</anchorfile>
      <anchor>afa5d4920d752c25aa211d50635a87b8d</anchor>
      <arglist>(const t_real max_u, const t_uint imsize, const t_real oversample_ratio)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>fov_cosine</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1widefield.html</anchorfile>
      <anchor>a1ea24a256a7408ced82c19ac8afb2de9</anchor>
      <arglist>(t_real const cell, t_uint const imsize)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>generate_chirp</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1widefield.html</anchorfile>
      <anchor>ad220bdbe5a36b5047dee3cc68f66533e</anchor>
      <arglist>(const t_real w_rate, const t_real cell_x, const t_real cell_y, const t_uint x_size, const t_uint y_size)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>generate_dde</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1widefield.html</anchorfile>
      <anchor>af8c53ba24d42ab813cbf4f03f56f2a7d</anchor>
      <arglist>(const DDE &amp;dde, const t_real cell_x, const t_real cell_y, const t_uint x_size, const t_uint y_size)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>generate_chirp</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1widefield.html</anchorfile>
      <anchor>a7dd1480057051aae3314ab9465821ad4</anchor>
      <arglist>(const DDE &amp;dde, const t_real w_rate, const t_real cell_x, const t_real cell_y, const t_uint x_size, const t_uint y_size)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify::wproj_utilities</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</filename>
    <namespace>purify::wproj_utilities::expansions</namespace>
    <member kind="function">
      <type>std::vector&lt; t_uint &gt;</type>
      <name>w_rows</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a028e2635db8ad772b8c14216349f4de2</anchor>
      <arglist>(const Sparse&lt; t_complex &gt; &amp;w_degrider)</arglist>
    </member>
    <member kind="function">
      <type>std::tuple&lt; t_real, t_real &gt;</type>
      <name>fov_cosines</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a55409e6c696934e139f47f0930285538</anchor>
      <arglist>(t_real const &amp;cell_x, t_real const &amp;cell_y, t_uint const &amp;x_size, t_uint const &amp;y_size)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>generate_dde</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a9c8340087c9a825336f8e1d511248d20</anchor>
      <arglist>(const std::function&lt; t_complex(t_real, t_real)&gt; &amp;dde, const t_real &amp;cell_x, const t_real &amp;cell_y, const t_uint &amp;x_size, const t_uint &amp;y_size)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>generate_chirp</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a44e1cf3c127592ab55a696769d40bc1b</anchor>
      <arglist>(const std::function&lt; t_complex(t_real, t_real)&gt; &amp;dde, const t_real &amp;w_rate, const t_real &amp;cell_x, const t_real &amp;cell_y, const t_uint &amp;x_size, const t_uint &amp;y_size)</arglist>
    </member>
    <member kind="function">
      <type>Matrix&lt; t_complex &gt;</type>
      <name>generate_chirp</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a2c06254253286f0e080fd31494c9afb3</anchor>
      <arglist>(const t_real &amp;w_rate, const t_real &amp;cell_x, const t_real &amp;cell_y, const t_uint &amp;x_size, const t_uint &amp;y_size)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>create_chirp_row</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a64fb8d0d62d0bccbd328c95e4146f510</anchor>
      <arglist>(const t_real &amp;w_rate, const t_real &amp;cell_x, const t_real &amp;cell_y, const t_uint &amp;ftsizev, const t_uint &amp;ftsizeu, const t_real &amp;energy_fraction, const sopt::OperatorFunction&lt; Vector&lt; t_complex &gt;&gt; &amp;fftop)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>create_chirp_row</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a72764af9467ca2f6ef54561861c71fdb</anchor>
      <arglist>(const Vector&lt; t_complex &gt; &amp;chirp_image, const t_real &amp;energy_fraction, const sopt::OperatorFunction&lt; Vector&lt; t_complex &gt;&gt; &amp;fftop)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>wprojection_matrix</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>af77eb45df90553de1b6d470cc2812d7a</anchor>
      <arglist>(const Sparse&lt; t_complex &gt; &amp;G, const t_uint &amp;x_size, const t_uint &amp;y_size, const Vector&lt; t_real &gt; &amp;w_components, const t_real &amp;cell_x, const t_real &amp;cell_y, const t_real &amp;energy_fraction_chirp, const t_real &amp;energy_fraction_wproj, const expansions::series series=expansions::series::none, const t_uint order=1, const t_real &amp;interpolation_error=1e-2)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>snr_metric</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a17c5b50902a73725b22c031f40bf1cae</anchor>
      <arglist>(const Image&lt; t_real &gt; &amp;model, const Image&lt; t_real &gt; &amp;solution)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>mr_metric</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>ac91e4de3e44bf6b1a10b266f2768ca3e</anchor>
      <arglist>(const Image&lt; t_real &gt; &amp;model, const Image&lt; t_real &gt; &amp;solution)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>generate_vect</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>ab2721935f8875cce834fd3f8d01cfd2e</anchor>
      <arglist>(const t_uint &amp;x_size, const t_uint &amp;y_size, const t_real &amp;sigma, const t_real &amp;mean)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>convert_sparse</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a7a85d11b4c738e9808ef35ce971174bc</anchor>
      <arglist>(const Eigen::MatrixBase&lt; T &gt; &amp;M, const t_real &amp;threshold=0.)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>sparsify_row_thres</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a4d084f0a41724ba4bcf53ffaff6f2968</anchor>
      <arglist>(const Eigen::SparseMatrixBase&lt; T &gt; &amp;row, const t_real &amp;energy)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>sparsify_row_dense_thres</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a5abb6ec3b503e160ad37487c1bfe34fd</anchor>
      <arglist>(const Eigen::MatrixBase&lt; T &gt; &amp;row, const t_real &amp;energy)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>row_wise_convolution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a77c1fcdca84805f84e0c7ae2f1b15746</anchor>
      <arglist>(const Sparse&lt; t_complex &gt; &amp;Grid_, const Sparse&lt; T &gt; &amp;chirp_, const t_uint &amp;x_size, const t_uint &amp;y_size)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>sparsity_sp</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a40d840c74c357373271393c1a3e814f6</anchor>
      <arglist>(const Sparse&lt; t_complex &gt; &amp;Gmat)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>sparsity_im</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>a9514b3dffe2921b9a6cdbe7f1f6d48e9</anchor>
      <arglist>(const Image&lt; t_complex &gt; &amp;Cmat)</arglist>
    </member>
    <member kind="function">
      <type>t_real</type>
      <name>upsample_ratio_sim</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>abd072be7657eca4ffa663c86f7d9af0c</anchor>
      <arglist>(const utilities::vis_params &amp;uv_vis, const t_real &amp;L, const t_real &amp;M, const t_int &amp;x_size, const t_int &amp;y_size, const t_int &amp;multipleOf)</arglist>
    </member>
    <member kind="function">
      <type>Sparse&lt; t_complex &gt;</type>
      <name>row_wise_sparse_convolution</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities.html</anchorfile>
      <anchor>ae9b2839aeb90d232c87cc0806491f84a</anchor>
      <arglist>(const Sparse&lt; t_complex &gt; &amp;Grid_, const Sparse&lt; T &gt; &amp;chirp_, const t_uint &amp;x_size, const t_uint &amp;y_size)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>purify::wproj_utilities::expansions</name>
    <filename>http://astro-informatics.github.io/purify/http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities_1_1expansions.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>series</name>
      <anchorfile>http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities_1_1expansions.html</anchorfile>
      <anchor>a92334b90f8de8f723450b3aca72d9e74</anchor>
      <arglist></arglist>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities_1_1expansions.html" anchor="a92334b90f8de8f723450b3aca72d9e74a334c4a4c42fdb79d7ebc3e73b517e6f8">none</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities_1_1expansions.html" anchor="a92334b90f8de8f723450b3aca72d9e74a7d8bc5f1a8d3787d06ef11c97d4655df">taylor</enumvalue>
      <enumvalue file="http://astro-informatics.github.io/purify/namespacepurify_1_1wproj__utilities_1_1expansions.html" anchor="a92334b90f8de8f723450b3aca72d9e74a172e4ecb02a864e1e4aa51dcce9d8a47">chebyshev</enumvalue>
    </member>
  </compound>
  <compound kind="page">
    <name>purify_contributors</name>
    <title>Contributors</title>
    <filename>http://astro-informatics.github.io/purify/purify_contributors</filename>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>PURIFY</title>
    <filename>http://astro-informatics.github.io/purify/index</filename>
  </compound>
</tagfile>
