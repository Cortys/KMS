"use strict";

const path = require("path");
const fs = require("fs");

function countChars(chars) {
	const charCount = {};

	chars.filter(char => /[A-Z]/i.test(char)).forEach(char => charCount[char] = charCount[char] + 1 || 1);

	return charCount;
}

function uppercaseifyChars(chars) {
	return chars.map(char => char === "ß" ? "ß" : char.toUpperCase());
}

function countCharsUppercase(chars) {
	return countChars(uppercaseifyChars(chars));
}

function charCountToCharOrder(charCount) {
	return Object.keys(charCount).sort((a, b) => charCount[b] - charCount[a]);
}

function lowercaseifyMap(map) {
	const entries = [...map];

	return new Map([...entries, ...entries.map(replacement => replacement.map(c => c.toLowerCase()))]);
}

function uppercaseifyMap(map) {
	const entries = [...map];

	return new Map([...entries.filter((([a, b]) => /[A-Z]{2,2}/.test(`${a}${b}`))).sort((a, b) => a[0].charCodeAt(0) - b[0].charCodeAt(0))]);
}

function toMap(...args) {
	return new Map(String.raw(...args).split(/[\n\t]+/).map(entry => entry.split(" ")).filter(entry => entry.length == 2));
}

function decipherText(text, replacementMap) {
	return text.split("").map(char => replacementMap.get(char) || char).join("");
}

function toPerc(val) {
	return `${Math.round(val * 10000) / 100}%`;
}

function mapDiff(a, b) {
	a = uppercaseifyMap(a);
	b = uppercaseifyMap(b);

	let count = 0;

	a.forEach((value, key) => b.get(key) !== value && count++);

	return `${a.size - count} / ${a.size} = ${toPerc(1 - (count / a.size))}`;
}

const replacementMethods = {
	allOrder: "ENISRATDHULCGMOBWFKZPVJYXQ".split(""),
	startOrder: "DSEIW".split(""),
	endOrder: "NERTS".split(""),

	simpleFrequency(text) {
		const charCount = countCharsUppercase(text.split(""));

		const replacements = charCountToCharOrder(charCount).map((char, i) => [char, this.allOrder[i]]);

		return lowercaseifyMap(replacements);
	},

	wordFrequency(text, knownReplacements) {
		const allCount = countCharsUppercase(text.split(""));
		const words = text.split(/[^a-zßäöü]/i);
		const startCount = countCharsUppercase(words.map(word => word.charAt(0)).filter(char => !!char));
		const endCount = countCharsUppercase(words.map(word => word.charAt(word.length - 1)).filter(char => !!char));

		const cipherOrder = {
			all: charCountToCharOrder(allCount),
			start: charCountToCharOrder(startCount),
			end: charCountToCharOrder(endCount)
		};

		const statOrder = {
			all: [...this.allOrder],
			start: [...this.startOrder],
			end: [...this.endOrder]
		};

		const freqOrder = ["end", "all", "end", "start", "end", "start", "end", "all", "end", "start", "all", "all", "start", "all", "start"];

		function removeItemFromArray(array, char) {
			const i = array.indexOf(char);

			if(i >= 0)
				array.splice(i, 1);

			return i;
		}

		function removeItemFromCipher(type, cipher) {
			removeItemFromArray(cipherOrder[type], cipher);
		}

		function removeItemFromStat(type, stat) {
			let i = removeItemFromArray(statOrder[type], stat);

			if(i >= 0) {
				let found = -1;
				for(const [key, value] of freqOrder.entries()) {
					if(value === type)
						found++;

					if(found === i) {
						freqOrder.splice(key, 1);
						break;
					}
				}
			}
		}

		if(!knownReplacements)
			knownReplacements = new Map();

		const replacements = new Map(knownReplacements);

		for(const [cipher, stat] of knownReplacements) {
			["all", "start", "end"].forEach(type => {
				removeItemFromCipher(type, cipher);
				removeItemFromStat(type, stat);
			});
		}

		while(cipherOrder.all.length > 0) {
			const type = freqOrder[0] || "all";
			const cipher = cipherOrder[type][0];
			const stat = statOrder[type][0];

			if(!cipher || !stat) {
				freqOrder.pop();

				return;
			}

			["all", "start", "end"].forEach(type => {
				removeItemFromCipher(type, cipher);
				removeItemFromStat(type, stat);
			});

			replacements.set(cipher, stat);
		}

		return lowercaseifyMap(replacements);
	}
};

const text = fs.readFileSync(path.join(__dirname, "text.txt"), {
	encoding: "utf8"
});

const charCount = countCharsUppercase(text.split(""));
const sum = Object.keys(charCount).reduce((sum, key) => sum + charCount[key], 0);
const freq = new Map(Object.keys(charCount).sort((a, b) => a.charCodeAt(0) - b.charCodeAt(0)).map(key => [key, toPerc(charCount[key] / sum)]));

console.log("Character frequency:", freq, "\n");

const simple = replacementMethods.simpleFrequency(text);
const word1 = replacementMethods.wordFrequency(text);
const word2 = replacementMethods.wordFrequency(text, toMap`
	F U
	S H
	N M
	T G
	C X
`);

console.log(`Simple (${mapDiff(word2, simple)} accuracy):`);
console.log(decipherText(text, simple));

console.log(`Advanced (${mapDiff(word2, word1)} accuracy):`);
console.log(decipherText(text, word1));

console.log(`Advanced with additional information (${mapDiff(word2, word2)} accuracy):`);
console.log(decipherText(text, word2));

console.log("Correct substitution table:", uppercaseifyMap(word2));
