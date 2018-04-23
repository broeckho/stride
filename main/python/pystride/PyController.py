'''
import os
from time import localtime, strftime
from .SimulationObserver import SimulationObserver
'''

import pystride
from .Config import Config
from .PyRunner import PyRunner

class PyController:
    pass

'''
    def __init__(self):
        self.forks = list()

        self.runner = PyRunner()
        #TODO list of observers

        self.runConfig = Config(root="run")
        self.diseaseConfig = Config(root="disease")

    def loadRunConfig(self, filepath: str):
        self.runConfig = Config(filepath)
        #self.diseaseConfig = Config(os.path.join(self.dataDir, self.runConfig.getParameter("disease_config_file")))
        #self.runConfig.setParameter("output_prefix", self.getLabel())

    def loadDiseaseConfig(self, filepath: str):
        pass

    def run(self):
        """ Run the current simulation. """
        self.runner.setup(self.runConfig)
        self.runner.run()

    def runForks(self):
        """
        """
        pass

    def runAll(self):
        """
        """
        pass
'''


'''
class Simulation:
    def __init__(self, dataDir=None):
        self.timestamp =  strftime("%Y%m%d_%H%M%S", localtime())
        if dataDir == None:
            self.dataDir = os.path.join("..", "data")
        else:
            self.dataDir = dataDir

    def loadDiseaseConfig(self, filename: str):
        self.diseaseConfig = Config(filename)
        self.runConfig.setParameter("disease_config_file", os.path.basename(filename))

    def getLabel(self):
        label = self.runConfig.getParameter('output_prefix')
        if label == None:
            return self.timestamp
        return label

    def getWorkingDirectory(self):
        return pystride.workspace

    def getOutputDirectory(self):
        return os.path.join(self.getWorkingDirectory(), self.getLabel()+"/")

    def _linkData(self):
        dataDestDir = os.path.join(self.getOutputDirectory(), "data")
        os.makedirs(dataDestDir, exist_ok=True)
        file_params = [
            "population_file",
            "holidays_file",
            "age_contact_matrix_file",
            # TODO disease_config_file?
        ]
        for param in file_params:
            src = os.path.join(self.dataDir, self.runConfig.getParameter(param))
            self.runConfig.setParameter(param, src)
            dst = os.path.join(dataDestDir, os.path.basename(src))
            if (os.path.isfile(src)) and (not (os.path.isfile(dst))):
                os.symlink(src, dst)

    def _setup(self, linkData=True):
        """
            Create folder in workspace to run simulation.
            Copy config and link to data
        """
        if linkData:
            self._linkData()

        os.makedirs(self.getOutputDirectory(), exist_ok=True)
        # Store disease configuration
        oldDiseaseFile = self.runConfig.getParameter("disease_config_file")[:-4]
        diseaseFile = oldDiseaseFile + "_" + self.getLabel() + ".xml"
        diseasePath = os.path.join(self.getOutputDirectory(), diseaseFile)
        self.diseaseConfig.toFile(diseasePath)
        self.runConfig.setParameter("disease_config_file", diseasePath)
        # Store the run configuration
        configPath = os.path.join(self.getOutputDirectory(), self.getLabel() + ".xml")
        oldLabel = self.getLabel()
        self.runConfig.setParameter("output_prefix", self.getOutputDirectory())
        self.runConfig.toFile(configPath)
        self.runConfig.setParameter('output_prefix', oldLabel)

    def registerCallback(self, callback):
        """ Registers a callback to the simulation. """
        self.observer.RegisterCallback(callback)

    def fork(self, name: str):
        """
            Create a new simulation instance from this one.
            :param str name: the name of the fork.
        """
        f = Fork(name, self)
        return f

    def run(self, trackIndexCase=False):
        """ Run the current simulation. """
        self._setup()

        configPath = os.path.join(self.getOutputDirectory(), self.getLabel() + ".xml")
        try:
            self.runner.Setup(trackIndexCase, configPath)
            self.runner.RegisterObserver(self.observer)
            self.runner.Run()
        except:
            print("Exception while running the simulator. Closing down.")
            exit(1)

    def runForks(self, *args, **kwargs):
        """ Run all forks but not the root simulation. """
        self._setup()
        for fork in self.forks:
            fork.run(*args, **kwargs)

    def runAll(self, *args, **kwargs):
        """ Run the root simulation and all forks. """
        self.run(*args, **kwargs)
        self.runForks(*args, **kwargs)

from .Fork import Fork'''


'''
/**
 * The CliController controls execution of a simulation run (@see SimRunner) from the
 * command line interface (cli).
 * The CliController setup
 * \li accepts the commandline arguments
 * \li checks the OpenMP environment
 * \li checks the file system environment
 * \li reads the config file specified on the cli
 * \li effects cli overides of config parameters
 * \li patches the config file for any remaining defaults
 * \li interprets and executes the ouput prefix
 * \li makes a stride logger
 * The CliController execution
 * \li creates a simulation runner (@see SimRunner)
 * \li registers the appropriate viewers
 * \li runs the simulation
 */
class CliController
{
public:
        /// Straight initialization.
        explicit CliController(const boost::property_tree::ptree& config_pt);

        /// Actual run of the simulator.
        void Control();

        /// Setup the controller.
        void Setup();

private:
        /// Check install environment.
        void CheckEnv();

        /// Check the OpenMP environment.
        void CheckOpenMP();

        // Output_prefix: if it's a string not containing any / it gets interpreted as a
        // filename prefix; otherwise we 'll create the corresponding directory.
        void CheckOutputPrefix();

        /// Make the appropriate logger for cli environment and register as stride_logger.
        void MakeLogger();

        /// Register the viewers of the SimRunner.
        void RegisterViewers(std::shared_ptr<SimRunner> runner);

private:
        std::string                                       m_config_file;      /// Config parameters file name.
        std::vector<std::tuple<std::string, std::string>> m_p_overrides;      /// Cli overides of config parameters.

        std::string m_stride_log_level; /// Log level (see spdlog::level in spdlog/common.h).
        bool        m_use_install_dirs; /// Working dir or install dir mode.

        unsigned int      m_max_num_threads; /// Max number  of OpenMP threads.
        std::string       m_output_prefix;   /// Prefix to output (name prefix or prefix dir)
        util::Stopwatch<> m_run_clock;       ///< Stopwatch for timing the computation.

        boost::filesystem::path         m_config_path;   ///< path to config file.
        boost::property_tree::ptree     m_config_pt;     ///< Main configuration for run and sim.
        std::shared_ptr<spdlog::logger> m_stride_logger; ///< General logger.
};
'''


'''
CliController::CliController(const ptree& config_pt)
    : m_config_file(""), m_p_overrides(), m_stride_log_level(), m_use_install_dirs(), m_max_num_threads(1U),
      m_output_prefix(""), m_run_clock("run_clock", true), m_config_path(), m_config_pt(config_pt),
      m_stride_logger(nullptr)
{
        m_stride_log_level = m_config_pt.get<string>("run.stride_log_level");
        m_output_prefix    = m_config_pt.get<string>("run.output_prefix");
        m_use_install_dirs = m_config_pt.get<bool>("run.use_install_dirs");
};

void CliController::CheckEnv()
{
        if (m_use_install_dirs) {
                auto log = [](const string& s) -> void { cerr << s << endl; };
                if (!FileSys::CheckInstallEnv(log)) {
                        throw runtime_error("CliController::CheckEnv> Install dirs not OK.");
                }
        }
}

void CliController::CheckOpenMP() { m_max_num_threads = ConfigInfo::NumberAvailableThreads(); }

void CliController::CheckOutputPrefix()
{
        if (FileSys::IsDirectoryString(m_output_prefix)) {
                try {
                        create_directories(m_output_prefix);
                } catch (exception& e) {
                        cerr << "CliController::Setup> Exception creating directory:  {}" << m_output_prefix << endl;
                        throw;
                }
        }
}

void CliController::Control()
{
        // -----------------------------------------------------------------------------------------
        // Instantiate SimRunner & register viewers & setup+execute the run.
        // -----------------------------------------------------------------------------------------
        // Necessary (i.o. local variable) because (quote) a precondition of shared_from_this(),
        // namely that at least one shared_ptr must already have been created (and still exist)
        // pointing to this. Shared_from_this is used in viewer notification mechanism.
        // auto runner = make_shared<SimRunner>();
        auto runner = SimRunner::Create();

        // -----------------------------------------------------------------------------------------
        // Register viewers do runner setup and the execute.
        // -----------------------------------------------------------------------------------------
        RegisterViewers(runner);
        runner->Setup(m_config_pt);
        runner->Run();
        m_stride_logger->info("CliController shutting down. Timing: {}", m_run_clock.ToString());

        // -----------------------------------------------------------------------------------------
        // Done.
        // -----------------------------------------------------------------------------------------
        spdlog::drop_all();
}

void CliController::MakeLogger()
{
        const auto l    = FileSys::BuildPath(m_output_prefix, "stride_log.txt");
        m_stride_logger = LogUtils::CreateCliLogger("stride_logger", l.string());
        m_stride_logger->set_level(spdlog::level::from_str(m_stride_log_level));
        m_stride_logger->flush_on(spdlog::level::err);
}

void CliController::RegisterViewers(shared_ptr<SimRunner> runner)
{
        // Command line viewer
        m_stride_logger->info("Registering CliViewer");
        const auto cli_v = make_shared<viewers::CliViewer>(m_stride_logger);
        runner->Register(cli_v, bind(&viewers::CliViewer::Update, cli_v, placeholders::_1));

        // Adopted viewer
        if (m_config_pt.get<bool>("run.output_adopted", false)) {
                m_stride_logger->info("registering AdoptedViewer,");
                const auto v = make_shared<viewers::AdoptedViewer>(m_output_prefix);
                runner->Register(v, bind(&viewers::AdoptedViewer::Update, v, placeholders::_1));
        }

        // Cases viewer
        if (m_config_pt.get<bool>("run.output_cases", false)) {
                m_stride_logger->info("Registering CasesViewer");
                const auto v = make_shared<viewers::CasesViewer>(m_output_prefix);
                runner->Register(v, bind(&viewers::CasesViewer::Update, v, placeholders::_1));
        }

        // Persons viewer
        if (m_config_pt.get<bool>("run.output_persons", false)) {
                m_stride_logger->info("registering PersonsViewer.");
                const auto v = make_shared<viewers::PersonsViewer>(m_output_prefix);
                runner->Register(v, bind(&viewers::PersonsViewer::Update, v, placeholders::_1));
        }

        // Summary viewer
        if (m_config_pt.get<bool>("run.output_summary", false)) {
                m_stride_logger->info("Registering SummaryViewer");
                const auto v = make_shared<viewers::SummaryViewer>(m_output_prefix);
                runner->Register(v, bind(&viewers::SummaryViewer::Update, v, placeholders::_1));
        }
}

void CliController::Setup()
{
        // -----------------------------------------------------------------------------------------
        // Check environment, deal with output_prefix (i.e. make the directory iff the
        // prefix contains at least one /, make a logger and register it.
        // -----------------------------------------------------------------------------------------
        CheckEnv();
        CheckOpenMP();
        CheckOutputPrefix();
        MakeLogger();
        spdlog::register_logger(m_stride_logger);

        // -----------------------------------------------------------------------------------------
        // Log the setup.
        // -----------------------------------------------------------------------------------------
        m_stride_logger->info("CliController stating up at: {}", TimeStamp().ToString());
        m_stride_logger->info("Executing revision {}", ConfigInfo::GitRevision());
        m_stride_logger->info("Using configuration file:  {}", m_config_path.string());
        m_stride_logger->debug("Creating dir:  {}", m_output_prefix);
        m_stride_logger->debug("Executing:           {}", FileSys::GetExecPath().string());
        m_stride_logger->debug("Current directory:   {}", FileSys::GetCurrentDir().string());
        if (m_use_install_dirs) {
                m_stride_logger->debug("Install directory:   {}", FileSys::GetRootDir().string());
                m_stride_logger->debug("Config  directory:   {}", FileSys::GetConfigDir().string());
                m_stride_logger->debug("Data    directory:   {}", FileSys::GetDataDir().string());
        }
        if (ConfigInfo::HaveOpenMP()) {
                m_stride_logger->info("Max number OpenMP threads in this environment: {}", m_max_num_threads);
                m_stride_logger->info("Configured number of threads: {}",
                                      m_config_pt.get<unsigned int>("run.num_threads"));
        } else {
                m_stride_logger->info("Not using OpenMP threads.");
        }
}
'''
