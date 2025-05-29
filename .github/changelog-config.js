'use strict'

const config = require('conventional-changelog-angular')

module.exports = config({
  types: [
    { type: 'feat', section: '🚀 Features' },
    { type: 'fix', section: '🐛 Bug Fixes' },
    { type: 'refactor', section: '🔧 Internal Changes' },
    { type: 'chore', section: '🔧 Internal Changes' },
    { type: 'docs', hidden: true },
    { type: 'style', hidden: true },
    { type: 'perf', hidden: true },
    { type: 'test', hidden: true }
  ],
  noteKeywords: ['BREAKING CHANGE', 'BREAKING CHANGES'],
  // 添加对带 ! 标记的提交的处理
  parserOpts: {
    headerPattern: /^(\w*)(?:\((.*)\))?!?: (.*)$/,
    breakingHeaderPattern: /^(\w*)(?:\((.*)\))?!: (.*)$/
  }
})
