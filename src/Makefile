# LION intro — build with UASM (brew tap upiter/uasm && brew install uasm)
# Compress with UPX (brew install upx)
# Run with DOSBox (brew install --cask dosbox)

UASM    ?= uasm
UPX     ?= upx
UASMFLAGS = -Zm -bin
TARGET  = lion.com
ASM     = LION.ASM
DEPS    = MACROS.INC PLASMA.INC ROTAZOOM.INC TUNNEL.INC 3D2.INC

.PHONY: build run clean

build: $(TARGET)

$(TARGET): $(ASM) $(DEPS)
	$(UASM) $(UASMFLAGS) -Fo $@ $(ASM)
	$(UPX) --best $@

run: $(TARGET)
	@PATH="/opt/homebrew/bin:/usr/local/bin:$$PATH"; \
	if command -v dosbox >/dev/null 2>&1; then \
		dosbox -exit "$(CURDIR)/$(TARGET)"; \
	elif [ "$$(uname)" = Darwin ] && [ -d /Applications/DOSBox.app ]; then \
		open -a DOSBox --args -c "mount c $(CURDIR)" -c "c:" -c "$(TARGET)"; \
	else \
		echo "DOSBox not found. Install: brew install --cask dosbox" >&2; \
		exit 1; \
	fi

clean:
	rm -f $(TARGET) lion.lst lion.obj
