var icu = require('./build/Release/icu');
var str = 'キョーハイーテンキダナー';

console.log(icu.kana2romaji(str));
console.log(icu.kana2voca(str));
