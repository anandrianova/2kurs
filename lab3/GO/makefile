.PHONY: all test cover lint bench clean help serialization-test binary-test format-compare run cli build-cli

# Default target
all: test lint

# Variables
COVERAGE_FILE = coverage.out
COVERAGE_HTML = coverage.html
BENCH_FILE = bench.out
MODULE_NAME = laba3go
GOLANGCI_LINT = $(shell go env GOPATH)/bin/golangci-lint
BINARY_NAME = containers

# Help
help:
	@echo "Available targets:"
	@echo "  all           - Run tests and lint (default)"
	@echo "  test          - Run all tests"
	@echo "  cover         - Run tests with coverage and generate HTML report"
	@echo "  cover-check   - Run tests and check 90% coverage threshold"
	@echo "  lint          - Run golangci-lint"
	@echo "  bench         - Run benchmarks"
	@echo "  clean         - Remove generated files"
	@echo "  build         - Build the application"
	@echo "  run           - Run the application in interactive mode"
	@echo "  cli           - Run CLI in interactive mode (alias for run)"
	@echo "  build-cli     - Build CLI application"
	@echo "  serialization-test - Test serialization formats"
	@echo "  binary-test   - Test binary serialization specifically"
	@echo "  format-compare - Compare JSON vs Binary serialization"

# Install dependencies
deps:
	@echo "Installing dependencies..."
	go mod tidy

# Install golangci-lint
$(GOLANGCI_LINT):
	@echo "Installing golangci-lint..."
	curl -sSfL https://raw.githubusercontent.com/golangci/golangci-lint/master/install.sh | sh -s -- -b $(shell go env GOPATH)/bin

# Run tests
test: deps
	@echo "Running tests..."
	go test -v -short ./...

# Run tests with coverage
cover: deps
	@echo "Running tests with coverage..."
	go test -v -coverprofile=$(COVERAGE_FILE) ./...
	@echo "\nCoverage summary:"
	go tool cover -func=$(COVERAGE_FILE)
	@echo "\nGenerating HTML report: $(COVERAGE_HTML)"
	go tool cover -html=$(COVERAGE_FILE) -o $(COVERAGE_HTML)

# Check coverage threshold (90%)
cover-check: cover
	@echo "Checking coverage threshold (90%)..."
	@coverage=$$(go tool cover -func=$(COVERAGE_FILE) | grep total | awk '{print substr($$3, 1, length($$3)-1)}'); \
	echo "Total coverage: $$coverage%"; \
	if [ "$${coverage%%.*}" -ge 90 ]; then \
		echo "✅ Coverage meets 90% threshold"; \
	else \
		echo "❌ Coverage below 90% threshold"; \
		exit 1; \
	fi

# Run lint
lint: $(GOLANGCI_LINT)
	@echo "Running golangci-lint..."
	$(GOLANGCI_LINT) run ./...

# Run benchmarks
bench: deps
	@echo "Running benchmarks..."
	go test -bench=. -benchmem -run=^$$ ./... | tee $(BENCH_FILE)

# Clean generated files
clean:
	@echo "Cleaning generated files..."
	rm -f $(COVERAGE_FILE) $(COVERAGE_HTML) $(BENCH_FILE) $(BINARY_NAME)
	go clean -testcache

# Build the application
build: deps
	@echo "Building application..."
	go build -o $(BINARY_NAME) .

# Build CLI application (alias for build)
build-cli: build

# Run the application in interactive mode
run: build
	@echo "Running application in interactive mode..."
	./$(BINARY_NAME)

# Run CLI in interactive mode (alias for run)
cli: run

# Run with single command
run-cmd: build
	@if [ -z "$(cmd)" ]; then \
		echo "Usage: make run-cmd cmd='FCREATE mylist'"; \
		exit 1; \
	fi
	./$(BINARY_NAME) --query "$(cmd)"

# Run from file
run-file: build
	@if [ -z "$(file)" ]; then \
		echo "Usage: make run-file file=commands.txt"; \
		exit 1; \
	fi
	./$(BINARY_NAME) --file "$(file)"

# Format code
fmt:
	go fmt ./...

# Vet code
vet:
	go vet ./...

# Full verification
verify: test cover-check lint
	@echo "✅ All checks passed!"

# Show project info
info:
	@echo "Project: $(MODULE_NAME)"
	@echo "Go version: $$(go version)"
	@echo "Binary: $(BINARY_NAME)"
	@echo "GolangCI-Lint: $$($(GOLANGCI_LINT) --version 2>/dev/null || echo 'Not installed')"

# Тестирование сериализации
serialization-test:
	@echo "Testing serialization formats..."
	go test -v -run ".*Serialization.*" ./...

# Тестирование бинарной сериализации  
binary-test:
	@echo "Testing binary serialization..."
	go test -v -run ".*Binary.*" ./...

# Сравнение форматов
format-compare: build
	@echo "Comparing serialization formats..."
	./$(BINARY_NAME) --query "HELP"

# Create example commands file
create-example:
	@echo "Creating example commands file..."
	@echo "# Example commands file for Containers CLI" > example_commands.txt
	@echo "# Create containers" >> example_commands.txt
	@echo "FCREATE mylist" >> example_commands.txt
	@echo "FCREATE myarray" >> example_commands.txt
	@echo "DCREATE mytable" >> example_commands.txt
	@echo "TCREATE mytree" >> example_commands.txt
	@echo "" >> example_commands.txt
	@echo "# Work with singly linked list" >> example_commands.txt
	@echo "FPUSHHEAD mylist Alice" >> example_commands.txt
	@echo "FPUSHTAIL mylist Bob" >> example_commands.txt
	@echo "FPUSHAFTER mylist Charlie Alice" >> example_commands.txt
	@echo "PRINT mylist" >> example_commands.txt
	@echo "SIZE mylist" >> example_commands.txt
	@echo "" >> example_commands.txt
	@echo "# Work with array" >> example_commands.txt
	@echo "MPUSH myarray first" >> example_commands.txt
	@echo "MPUSH myarray second" >> example_commands.txt
	@echo "MPUSH myarray third" >> example_commands.txt
	@echo "PRINT myarray" >> example_commands.txt
	@echo "" >> example_commands.txt
	@echo "# Work with hash table" >> example_commands.txt
	@echo "DINSERT mytable 1 John" >> example_commands.txt
	@echo "DINSERT mytable 2 Jane" >> example_commands.txt
	@echo "DSEARCH mytable 1" >> example_commands.txt
	@echo "PRINT mytable" >> example_commands.txt
	@echo "" >> example_commands.txt
	@echo "# Work with binary tree" >> example_commands.txt
	@echo "TINSERT mytree apple" >> example_commands.txt
	@echo "TINSERT mytree banana" >> example_commands.txt
	@echo "TINSERT mytree cherry" >> example_commands.txt
	@echo "PRINT mytree" >> example_commands.txt
	@echo "" >> example_commands.txt
	@echo "# List all containers" >> example_commands.txt
	@echo "LIST" >> example_commands.txt
	@echo "Example commands file created: example_commands.txt"

# Quick demo
demo: build create-example
	@echo "Running demo..."
	./$(BINARY_NAME) --file example_commands.txt

# Install (build and show info)
install: build info
	@echo "✅ Application installed successfully!"
	@echo "💡 Usage:"
	@echo "  ./$(BINARY_NAME)              # Interactive mode"
	@echo "  ./$(BINARY_NAME) --query HELP # Single command"
	@echo "  ./$(BINARY_NAME) --file cmds.txt # From file"

# Development mode (auto-reload on changes, requires air: go install github.com/cosmtrek/air@latest)
dev:
	@if command -v air >/dev/null 2>&1; then \
		air; \
	else \
		echo "Air not installed. Installing..."; \
		go install github.com/cosmtrek/air@latest; \
		air; \
	fi

# Performance test
perf-test: build
	@echo "Performance testing..."
	@time ./$(BINARY_NAME) --query "FCREATE test" > /dev/null
	@echo "✅ Basic functionality test passed"

# Security check (basic)
security-check:
	@echo "Running basic security checks..."
	go vet ./...
	@if command -v gosec >/dev/null 2>&1; then \
		gosec ./...; \
	else \
		echo "⚠️  gosec not installed, skipping. Install with: go install github.com/securego/gosec/v2/cmd/gosec@latest"; \
	fi

# Dependency audit
audit:
	@echo "Auditing dependencies..."
	go list -m all | grep -v laba3go
	go mod verify

# Release build (stripped and optimized)
release: deps
	@echo "Building release version..."
	CGO_ENABLED=0 GOOS=linux GOARCH=amd64 go build -ldflags="-s -w" -o $(BINARY_NAME)_linux_amd64 .
	CGO_ENABLED=0 GOOS=darwin GOARCH=amd64 go build -ldflags="-s -w" -o $(BINARY_NAME)_darwin_amd64 .
	CGO_ENABLED=0 GOOS=windows GOARCH=amd64 go build -ldflags="-s -w" -o $(BINARY_NAME)_windows_amd64.exe .
	@echo "✅ Release builds created:"
	@ls -la $(BINARY_NAME)_*

# Size check
size: build
	@echo "Binary size:"
	@ls -lh $(BINARY_NAME) | awk '{print $$5}'

# Quick test (fastest)
quick-test:
	@echo "Running quick tests..."
	go test -short ./...

# Memory profile
mem-profile: build
	@echo "Running with memory profiling..."
	go test -bench=. -memprofile=mem.prof -run=^$$ ./...
	@echo "Memory profile saved to mem.prof"

# CPU profile
cpu-profile: build
	@echo "Running with CPU profiling..."
	go test -bench=. -cpuprofile=cpu.prof -run=^$$ ./...
	@echo "CPU profile saved to cpu.prof"
