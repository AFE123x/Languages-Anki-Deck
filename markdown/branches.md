# Branching strategy

## Two branching workflow models

- `Mainline development`: always integrate from your work:
    - few branches, small commits, high quality testing & QA standards.
- `state, release and feature branch`: there are different branches for different kind of jobs (feature, develop, releases, etc.).

## long running vs. short lived branches

- **long running branches** exist through the complete lifetime of project (main, master, etc.)
    - These branches usually mirror "stages" in your dev life cycle (states in release, deployment). 
    - commits are never directy added to branches. They're usually done with rebase.

- **short lived branches** are created for certain purposes, and deleted when features integrated.
    - for example, features, bug fixes, refactoring, etc.
    - branches are usually based on the long running branch.

## pull requests - why?

- They offer a means to communicate about and reviewing code.
- When you are making complex changes before merging, you make a pull request.
    - There'll be a person that'll check the request, and if approved, will be merged to the main branch.

## pull requests - forks

- forks are your own personal copy of a git repository.

- here, you'd make the changes to your own version, and make a pull request to the original repo.


