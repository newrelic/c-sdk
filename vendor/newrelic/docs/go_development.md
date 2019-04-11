### Go References

#### [Effective Go](http://golang.org/doc/effective_go.html)

Effective Go is an example based guide to making best use of the
language and its libraries.

#### [Go Project Code Review Guidelines](https://github.com/golang/go/wiki/CodeReviewComments)

This the effective style guide for the Go project itself. It's a short
document covering topics from documentation, comments, naming, and writing
good tests. Many of these guidelines have been codified in the tooling.

#### [Go Blog](http://blog.golang.org/)

Blog posts on Go from the Go authors. Lots of good stuff here. Mike
recommends:
- [Errors are Values](http://blog.golang.org/errors-are-values)
- [The Cover Story](http://blog.golang.org/cover)
- [Strings, Runes, and Characters](http://blog.golang.org/strings)
- [Arrays and Slices](http://blog.golang.org/slices)
- [Defer, Panic and Recover](http://blog.golang.org/defer-panic-and-recover).

#### [Go Talks](http://talks.golang.org/)

A Directory listing of talks given by the Go authors. Links go to the
slides. Often, if a video is available, it will be linked from this
site or from one the first couple of slides. Slides are HTML based
and easily viewed within a browser. Mike recommends:
- [Lexical Scanning in Go (2011)](https://talks.golang.org/2011/lex.slide#1)
- [Real World Go (2011)](http://talks.golang.org/2011/Real_World_Go.pdf)
- [Go Concurrency Patterns (2012)](http://talks.golang.org/2012/concurrency.slide#1)
- [Advanced Concurrency Patterns (2013)](http://talks.golang.org/2013/advconc.slide#1)
- [Go Best Practices (2013)](http://talks.golang.org/2013/bestpractices.slide#1)

#### [Standard Library](https://github.com/golang/go/tree/master/src)

Use the source, Luke! The Go standard library is full of great and
sometimes subtle uses of the language that are worth learning from.

### Go Tools

- [Goimports](https://godoc.org/golang.org/x/tools/cmd/goimports) -- A
drop-in replacement for gofmt, but intended to be integrated with your
editor of choice. Highly recommended.
- [Go vet](https://godoc.org/cmd/vet) -- Ships as
part of the Go distribution. Go vet looks for semantic errors not caught
by the compiler.
- [Golint](https://github.com/golang/lint) -- A basic linter for Go
programs. Incorporates much of the advice in Effective Go and the Go
code review guidelines. Needs to be installed separately.
