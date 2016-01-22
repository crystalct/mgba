/* Copyright (c) 2013-2016 Jeffrey Pfau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "io.h"

#include "gb/gb.h"

void GBIOInit(struct GB* gb) {
	memset(gb->memory.io, 0, sizeof(gb->memory.io));
}

void GBIOReset(struct GB* gb) {
	memset(gb->memory.io, 0, sizeof(gb->memory.io));

	GBIOWrite(gb, 0x05, 0);
	GBIOWrite(gb, 0x06, 0);
	GBIOWrite(gb, 0x07, 0);
	GBIOWrite(gb, 0x10, 0x80);
	GBIOWrite(gb, 0x11, 0xBF);
	GBIOWrite(gb, 0x12, 0xF3);
	GBIOWrite(gb, 0x12, 0xF3);
	GBIOWrite(gb, 0x14, 0xBF);
	GBIOWrite(gb, 0x16, 0x3F);
	GBIOWrite(gb, 0x17, 0x00);
	GBIOWrite(gb, 0x19, 0xBF);
	GBIOWrite(gb, 0x1A, 0x7F);
	GBIOWrite(gb, 0x1B, 0xFF);
	GBIOWrite(gb, 0x1C, 0x9F);
	GBIOWrite(gb, 0x1E, 0xBF);
	GBIOWrite(gb, 0x20, 0xFF);
	GBIOWrite(gb, 0x21, 0x00);
	GBIOWrite(gb, 0x22, 0x00);
	GBIOWrite(gb, 0x23, 0xBF);
	GBIOWrite(gb, 0x24, 0x77);
	GBIOWrite(gb, 0x25, 0xF3);
	GBIOWrite(gb, 0x26, 0xF1);
	GBIOWrite(gb, 0x40, 0x91);
	GBIOWrite(gb, 0x42, 0x00);
	GBIOWrite(gb, 0x43, 0x00);
	GBIOWrite(gb, 0x45, 0x00);
	GBIOWrite(gb, 0x47, 0xFC);
	GBIOWrite(gb, 0x48, 0xFF);
	GBIOWrite(gb, 0x49, 0xFF);
	GBIOWrite(gb, 0x4A, 0x00);
	GBIOWrite(gb, 0x4B, 0x00);
	GBIOWrite(gb, 0xFF, 0x00);
}

void GBIOWrite(struct GB* gb, unsigned address, uint8_t value) {
	switch (address) {
	case REG_DIV:
		GBTimerDivReset(&gb->timer);
		return;
	case REG_TIMA:
		// ???
		return;
	case REG_TMA:
		// Handled transparently by the registers
		break;
	case REG_TAC:
		value = GBTimerUpdateTAC(&gb->timer, value);
		break;
	case REG_IF:
		gb->memory.io[REG_IF] = value | 0xE0;
		GBUpdateIRQs(gb);
		return;
	case REG_LCDC:
		// TODO: handle GBC differences
		value = gb->video.renderer->writeVideoRegister(gb->video.renderer, address, value);
		GBVideoWriteLCDC(&gb->video, value);
		break;
	case REG_DMA:
		GBMemoryDMA(gb, value << 8);
		break;
	case REG_SCY:
	case REG_SCX:
	case REG_WY:
	case REG_WX:
	case REG_BGP:
	case REG_OBP0:
	case REG_OBP1:
		value = gb->video.renderer->writeVideoRegister(gb->video.renderer, address, value);
		break;
	case REG_STAT:
		GBVideoWriteSTAT(&gb->video, value);
		break;
	case REG_IE:
		gb->memory.ie = value;
		GBUpdateIRQs(gb);
		return;
	default:
		// TODO: Log
		if (address >= GB_SIZE_IO) {
			return;
		}
		break;
	}
	gb->memory.io[address] = value;
}

static uint8_t _readKeys(struct GB* gb) {
	uint8_t keys = *gb->keySource;
	switch (gb->memory.io[REG_JOYP] & 0x30) {
	case 0x20:
		keys >>= 4;
		break;
	case 0x10:
		break;
	default:
		// ???
		keys = 0;
		break;
	}
	return 0xC0 | (gb->memory.io[REG_JOYP] | 0xF) ^ (keys & 0xF);
}

uint8_t GBIORead(struct GB* gb, unsigned address) {
	switch (address) {
	case REG_JOYP:
		return _readKeys(gb);
	case REG_IF:
		break;
	case REG_IE:
		return gb->memory.ie;
	case REG_DIV:
	case REG_TIMA:
	case REG_TMA:
	case REG_TAC:
	case REG_LY:
		// Handled transparently by the registers
		break;
	default:
		// TODO: Log
		if (address >= GB_SIZE_IO) {
			return 0;
		}
		break;
	}
	return gb->memory.io[address];
}
