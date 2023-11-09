# Per-User Configuration

To customize the configuration for all models, create a
`users/<username>/config.h` file.

To customize the configuration for a specific model, create a
`users/<username>/<manufacturer>/<model>/config.h` file (same directory
structure as that under `hw`, just replace `hw` with `users/<username>`).

Both customization methods can be used together, ie. you can have a global
`config.h` with all your favorite settings and a model-specific `config.h`
to override some of them on a specific light.

In the global or model-specific `config.h` you can override default settings
like this:

    #undef RAMP_SMOOTH_FLOOR
    #define RAMP_SMOOTH_FLOOR    30

To use these customizations, set the USER variable when running `make`:

    USER=username ./make [target]

If you place customizations in a directory under `users/` that matches your
Unix username (ie. `users/<your username>`), they will be used automatically
and you don't need to specify the username in the `make` command.
