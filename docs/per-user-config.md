# Per-User Configuration

To customize the configuration for all models, create a directory under
`users/<username>`.

Custom settings for any UI that implements FSM and this repository's build
tools can be configured as a header file under the UI's name. For example,
for Anduril, users/<username>/anduril.h. At the moment, only Anduril is
supported, but other UIs would be configurable in the same way.

To customize the configuration for a specific model, create
`users/<username>/models/<model name>/anduril.h`. The same principle as with
global user configuration here - other UIs can be implemented for the same light
in the future. The model name can be found from `MODELS` in the repo root.

Both customization methods can be used together, i.e. you can have a global
`anduril.h` with all your favorite settings and a model-specific `anduril.h`
to override some of them on a specific light.

```
$ find users/
users/
users/user1
users/user1/anduril.h
users/user1/model
users/user1/model/wurkkos-ts10
users/user1/model/wurkkos-ts10/anduril.h
users/user2
users/user2/anduril.h
users/user2/model
users/user2/model/wurkkos-ts10
users/user2/model/wurkkos-ts10-rgbaux/anduril.h
```

In the global or model-specific `config.h` you can override default settings
like this:

    #undef RAMP_SMOOTH_FLOOR
    #define RAMP_SMOOTH_FLOOR    30

To use these customizations, `USER` can be passed as an argument when running `make`:

    ./make [target] --user ExampleUser

More than one user can also be passed at once, e.g.:

    ./make [target] --user ExampleUser --user foo

You can configure the build scripts to *always* apply your user changes by
creating a file, `users.cfg` in the repository root, with the content of the file
being the names of the users under `users/`. If this configuration file is present,
it can be overridden by running `make` with `--no-user`.

```
$ cat << EOF > users.cfg
user1
user2
EOF

$ cat users.cfg
user1
user2

$ ./make ts10
Reading custom users from users.cfg; to skip use --no-user
Loaded user config dir user1 from users.cfg
Loaded user config dir user2 from users.cfg
```

User modified builds will be named with the user's name, for example
`anduril.wurkkos-ts10_ExampleUser.hex`. In addition, the version check string will
be generated with an additional dash (flicker) on the end to show that the build
includes user modifications, e.g. `0714-2024-04-05-`.

## User configuration management

There are seerval possible ways to manage your user configuration:

### Directly committing to repository

If you have forked the anduril repository, you can simply add your user configuration
to git and track it as any other file. This is the simplest method for managing
custom user configuration if you do not intend to make code contributions
to the upstream Anduril codebase.

### git submodule

TODO
