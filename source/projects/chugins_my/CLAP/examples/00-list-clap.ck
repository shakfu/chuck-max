// List all available CLAP plugins on the system
// This is useful for discovering what CLAP plugins are available

// Create CLAP instance
CLAP clap => blackhole;

// This will print all available CLAP plugins with their paths
clap.list();
