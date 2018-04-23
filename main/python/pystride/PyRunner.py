from pystride.stride.stride import SimulatorBuilder

class PyRunner:
    """
        Class responsible for the actual simulation loop.
        Functions as the 'model' in the MVC pattern.
    """
    def __init___(self):
        self.simulator = Simulator()
        # TODO clock
        # TODO logger?
        # TODO log_level?
        # TODO output_prefix
        # TODO configuration

    # TODO getClock()?
    # TODO getConfig() ?

    # TODO getSimulator()

    def setup(self, config_path):
        pass

    def run(self):
        # TODO clock.Start
        # TODO get num days
        # TODO log start?
        # TODO notify observers that sim is about to start
        #for day in range(num_days):
        #   TODO self.sim->TimeStep()
        #   TODO log time step
        #   TODO notify observers of timestep stepped

        # TODO notify observers that sim is Finished
        # TODO stop clock
        # TODO log finished ?
        pass


    '''
bool SimRunner::Setup(const ptree& config_pt)
{
        Notify({shared_from_this(), Id::SetupBegin});

        // -----------------------------------------------------------------------------------------
        // Intro.
        // -----------------------------------------------------------------------------------------
        m_clock.Start();
        bool status     = true;
        m_config_pt     = config_pt;
        m_log_level     = m_config_pt.get<string>("run.stride_log_level", "info");
        m_output_prefix = m_config_pt.get<string>("run.output_prefix");

        // -----------------------------------------------------------------------------------------
        // Unless execution context has done so, create logger, do NOT register it.
        // -----------------------------------------------------------------------------------------
        m_stride_logger = spdlog::get("stride_logger");
        if (!m_stride_logger) {
                const auto l    = FileSys::BuildPath(m_output_prefix, "stride_log.txt");
                m_stride_logger = LogUtils::CreateFileLogger("stride_logger", l.string());
                m_stride_logger->set_level(spdlog::level::from_str(m_log_level));
                m_stride_logger->flush_on(spdlog::level::err);
        }
        m_stride_logger->info("SimRunner setup starting at: {}", TimeStamp().ToString());

        // -----------------------------------------------------------------------------------------
        // Log the full run config.
        // -----------------------------------------------------------------------------------------
        ostringstream ss;
        write_xml(ss, m_config_pt, xml_writer_make_settings<ptree::key_type>(' ', 8));
        m_stride_logger->debug("Run config used:\n {}", ss.str());

        // ------------------------------------------------------------------------------
        // Build simulator.
        //------------------------------------------------------------------------------
        m_stride_logger->trace("Building the simulator.");
        SimulatorBuilder builder(m_config_pt, m_stride_logger);
        m_sim = builder.Build();
        m_stride_logger->trace("Done building the simulator.");

        // -----------------------------------------------------------------------------------------
        // Done.
        // -----------------------------------------------------------------------------------------
        m_clock.Stop();
        Notify({shared_from_this(), Id::SetupEnd});
        m_stride_logger->trace("Finished SimRunner::Setup.");
        return status;
}
    '''
