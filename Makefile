CC := gcc

TEST_DIR := test/spy_interrupt
TEST_DIR := test/spy_fakefunctions
TEST_DIR := test/alarm
# ^ this one is the one that is tested
TEST_SRC := $(wildcard $(TEST_DIR)/*.c)

SRCDIRS := src \
		$(TEST_DIR) \
		mocks \
		Unity/src
SRCS := $(foreach SRCDIR, $(SRCDIRS), $(wildcard $(SRCDIR)/*.c))

INCDIRS := include \
			Unity/src \
			mocks
INCFLAG := $(foreach INC, $(INCDIRS), -I$(INC))

FLAGS := -Wall -O0 -pedantic -Wextra $(INCFLAG)

all: debug test

debug:
	@echo SRCS = $(SRCS)
	@echo INCFLAG = $(INCFLAG)
	@echo TEST_SRC = $(TEST_SRC)

test: $(SRCS)
	@ruby Unity\auto\generate_test_runner.rb $(TEST_SRC)
	@echo "Will call: $(CC) $(FLAGS) $(SRCS) -o outTest.exe && ./outTest.exe"
	@$(CC) $(FLAGS) $(SRCS) -o outTest.exe && ./outTest.exe

clean:
	rm -rf *.exe