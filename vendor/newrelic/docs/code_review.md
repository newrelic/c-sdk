# PHP Agent Code Review
Code review in the PHP Agent Team is a reasonably painless process. This
process came out of a mixture of accidental convention and a Skype
conversation, so be aware that it's not terribly rigid — improvements and
suggestions are more than welcome.

Here's how it works:

1. New features or significant cleanups should be developed on a branch. Push
   commits to GitHub as you go. All PR's should include a reference to a JIRA
   issue (if one is applicable).
2. Once you think your work is done enough that it might be worth having
   someone else look at it (it doesn't have to be done at this point), issue a
   pull request, noting what you've done and what is not yet done -- it will
   help your team if you outline the basic architecture and areas you think
   might break. This will allow the rest of the team to put more eyes on your
   work and potentially catch architectural issues early.
3. When your code is ready, add the "ready for review" label. Make sure that
   there aren't any merge conflicts against the current master that have
   cropped up since you've been working.
4. You may also choose whether to squash the history (via [`git rebase
   -i`](http://git-scm.com/book/en/Git-Tools-Rewriting-History) or similar)
   before issuing the pull request. The general consensus of the team is that
   this isn't required in the general case — it's an annoying process and
   potentially loses history — but if you have a particularly messy commit
   history and want to simplify it for the reviewer(s), you can do so if you
   want.
   - If you choose to rebase when issuing "ready for review", make sure this is
     clear.
5. GitHub Enterprise will send an email to the team, letting them know that the
   pull request has been submitted. If you go a few days without review, then
   pick someone who doesn't seem totally stressed out and ask them to review
   your pull request. You may want to sweeten the deal with pie.
6. Once your reviewer is convinced that the code meets our standards, they will
   approve the PR (per [company policy][sidekick]) and you are clear to merge.
   If changes are requested, make them or discuss why they're unnecessary, get
   re-approved if any changes are made, and merge!

Our team's feeling is that it's better to submit pull requests early; add the
"ready for review" label when you're confident the pull request is ready. You
can use the "prototype" label to more strongly indicate that your PR isn't
ready for prying eyes, if needed.

The main PR comment can hold a living document that changes as changes are
added to the pull request, but you may want to consider keeping old and
outdated information with a strikethrough rather than deleting it, so as to
preserve information.

## Code Review Changes
Reviewers will post their reviews as GitHub comments — either line comments for
specific items, or as a comment on the pull request as a whole for broader
issues.

## Approving and Merging
We used to use comments with "+1" or "lgtm" for code review; now things are a
little bit more complicated. Anyone is welcome to review a pull request, but
one person must [sidekick the PR][sidekick] by approving the PR. Doing so means
that you have reviewed the PR and believe it to be of high enough quality to
join our codebase. Alas, this label only applies to the commit on which it was
applied, so even minor changes must be re-approved, per company policy.

The reviewer(s) may also ask that you get them to re-review a change before
merging to master. This is likely for more fundamental issues. Make the change,
update the pull request, and then ask nicely for a second look.

This document can't cover everything, including casees where you need to do
something different to do the right thing for our customers and company. Use
your own best judgment.

## Expectations for Authors
* Your code should be ready to merge: you should have performed reasonable QA
  before issuing the pull request.
* Your code **must** include a `CHANGELOG.md` entry if it includes any public
  or internal facing changes. PRs that only touch tests and/or documentation
  are exempt from this.
* Check that the PR builder passes. If it's failing due to flaky tests, adding
  an "ok jenkins" comment in the PR will restart the test build. If you really
  can't get an intermittent failure to stop, you can still merge, but make sure
  it's a testing and not a code error, and that the other tests are passing.
* Be understanding if it takes a little while for a review to happen,
  particularly on bigger features. Everyone's busy!

## Expectations for Reviewers
* Check the code thoroughly for errors. This could mean:
  - Reading every line of the PR.
  - Performing end-to-end tests using the UI.
  - Testing newly added features and configuration.
  - Considering edge cases.
  - Thinking about where the code interacts with other parts of our agent, a
    user's sytem, and our backend and whether we've upheld our contract.
  - Reading any applicable specs.
  - Asking for documentation.
  - Reading closely to make sure that we haven't leaked memory or used after a
    free.
* Double-check that the PR builder is passing (and you get that nice green
  light).
* Check for style guide violations. Spaces before parentheses might be weird,
  but it's better if we're all weird together.
* Be nice. Nobody likes their baby being called ugly. (Even if they do all look
  like wrinkly Winston Churchills.)

This document can't cover everything, including casees where you need to do
something different to do the right thing for our customers and company. Use
your own best judgment.

Have a nice day!

[sidekick]: https://pages.datanerd.us/engineering-management/processes/software-development-process/sidekick.html "Sidekick: Change control and quality review"
