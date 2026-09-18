This guide is meant to provide reference for some administration tasks in regards to account setup and management

The intent is to develop on a [VSCode server](https://code.visualstudio.com/docs/remote/vscode-server) on the respective projects build Pi, for easy compilation (you build directly on the build Pi). The build Pi will be on the same local network as the flight Pi, meaning you can easily transfer files over.

Each person developing should have their own user on the build Pi to separate development environments.

### Account creation

#### Abbreviated process

```bash
sudo useradd -m -s /bin/bash [USERNAME] # create the user
sudo passwd [USERNAME] # enter temporary password that will be sent to the user
sudo passwd [USERNAME] --expire # set the temp password to be expired
```

#### Explanation

To create an account for another user use the `useradd` command

```bash
sudo useradd -m -s /bin/bash [USERNAME]
```

The command flags used here are:

`-m, --create-home` : This creates a home directory for the user

`-s, --shell [SHELL]` : This sets the default shell to `[SHELL]`, often `/bin/bash`. The default is `/bin/sh`

Create a temporary password

```bash
sudo passwd [USERNAME]
sudo passwd [USERNAME] --expire
```

Expiring the password will force the user to create their own password when they attempt to login for the first time
