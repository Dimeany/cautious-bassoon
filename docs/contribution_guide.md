# Contribution Guide

For a good head start please read all documents in the `docs` folder (10-15min). They are brief, and provide a good overview of the project.

## Basic contribution flow

Here are some basic git commands for making contributions. This guide assumes you are using the build Pi(s) as your environment, and already have a user on the build Pi and know how to ssh in.

### Git flow

To start developing, first clone the Git repository and enter its directory. There are instructions to do this on the readme, reference that for the most up-to-date information.

Now, create an appropriately-named branch for your development.  If you are adding EPS status packets, you might run:

```bash
git checkout -b add-eps-status-packets
```

This `git checkout -b` command should only be run with the `-b` flag on one machine.  The `-b` flag tells `git checkout` to create the branch before switching to it; to merely switch to an existing branch run `git checkout add-eps-status-packets` (without the `-b` flag).

Develop the code, and commit as usual.  The first time you push to Github, you have to specify the remote (`origin`) and your branch name.  These can be saved using the `-u` flag:

```bash
git push -u origin add-eps-status-packets
```

When you push afterward, you can just run:

```bash
git push
```

(If you receive an error like `fatal: The current branch add-eps-status-packets has no upstream branch.` then you must give the full `git push -u ...` command, and subsequently you can use just `git push`.)

Assuming you have already created a branch and pushed it to Github from your development machine, you can switch to it on the Pi using:

```bash
git fetch
git checkout add-eps-status-packets
```

This will pull the latest information about the repo from Github, and then switch to the branch `add-eps-status-packets`. To actually pull the latest changes, you must run:

```bash
git pull
```

### Merging to main

The main branch has a protection rule that requires pull requests (PRs) for all changes.  A PR must be approved by at least one reviewer before it can be merged into `main`.  Getting reviews from more people is not required, but encouraged.  These requirements are meant to foster good contribution habits, create stability in the repo, and encourage people to review each other's code.

Before you make a PR, please test your changes on a Pi.  When you are happy with your changes, go to the Github website and create a Pull Request. Describe your changes in the pull request so it's easier to understand the point of the PR.

If there is a Github issue associated with your PR, you can refer to it in your commit message, commit body, or PR description, by typing a pound sign (`#`) followed by the issue number.  This will become a hyperlink to the Github issue.  Likewise, in an issue, you can refer to a PR by typing a pound sign followed by the PR number.  For example, if my PR fixes issue #10, I might include "Fixes #10" in the PR description.  More information about linking PRs and issues is available [here](https://docs.github.com/en/issues/tracking-your-work-with-issues/linking-a-pull-request-to-an-issue).

After your PR has been merged, you can switch back to the `main` and pull the latest changes, including your code, onto any machine by running:

```bash
git checkout main
git pull
```

Happy developing!

## Git

If you are new to git or would like a refresher [here is a visual and interactive tutorial](https://learngitbranching.js.org/)

Or if you prefer, dive into the [reference documentation](https://git-scm.com/docs) or have a [handy cheat sheet](https://training.github.com/downloads/github-git-cheat-sheet/)

### User Config Setup

When contributing with git you should configure your username and contribution email. This can be checked with `git config user.name && git config user.email`.

Your contribution email can be configured either globally within your computer with

```bash
git config --global user.name [x500]
git config --global user.email [x500@umn.edu]
```

or if you'd like to only configure for this repo that can be done while in the project folder with

```bash
git config --local user.name [x500]
git config --local user.email [x500@umn.edu]
```

## Contributing to Documentation

When making changes in the codebase consider updating the documentation to reflect those changes. Keeping the docs up to date with the code helps keep everyone on the same page.

Github has [reference](https://docs.github.com/en/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax) for markdown syntax.
