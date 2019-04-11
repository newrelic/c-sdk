# Guidelines for commit messages

This is basically a stripped-down version of the article [How to Write a Git Commit Message](https://chris.beams.io/posts/git-commit).

## Rules for good style

1. **Begin the message with a single short (less than 50 character) line summarizing the change.**

   That's what `man git commit` recommends. Many interfaces (`git log` as well 
   as UI parts of GitHub) are designed with this in mind.

2. **Separate subject from body with a blank line.**

   Again, recommended by `man git commit`. The first line of every commit 
   message is treated as the commit message title, all other lines separated 
   from the title by a blank line are treated as commit message body.

3. **Use the body to explain _what_ and _why_ vs. _how_.**

   More detailed explanations in the commit body might be desirable, as we will 
   loose the ability to provide references to JIRA issues or to pull request ids 
   from our private repository.

4. **Commit messages should comply to our source availability guidelines.**

   So, to reiterate the most important points:

   * no offensive language
   * no reference to NRCAMP or NRLAMP
   * no person names, initials and product codenames
   * no JIRA ticket numbers
   * no TODOs

## Recommendations

* Squash commits when merging in GitHub. This way, one needn't worry about
  commit message style during working on the PR, one only needs to write one
  well formatted commit message when merging.

* The PR title and description might give a good clue of what a meaningful 
  commit title and body should contain.

## Example

```
commit d3c264c91e31a517251692fa297aa3c70dd17e7f
Author: A Dev <adev@newrelic.com>
Date:   Tue Mar 27 11:46:56 2019 -0700

    Add commit message guidelines

    An informal and minimal set of commit message guidelines is provided at 
    a location only visible to internal developers.

    Their intent is to ensure, that our commit messages, which will be 
    visible to the public, conform to all our open source policies.

    Those guidelines were discussed and approved by the team.
```
