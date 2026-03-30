-- ===========================
--         ОСНОВНЫЕ НАСТРОЙКИ
-- ===========================
vim.g.mapleader = " "
vim.opt.number = true
vim.opt.relativenumber = true
vim.opt.tabstop = 4
vim.opt.shiftwidth = 4
vim.opt.expandtab = true
vim.opt.termguicolors = true
vim.opt.cursorline = true
vim.opt.mouse = "a"
vim.opt.updatetime = 300
vim.opt.clipboard = "unnamedplus" -- общий буфер обмена (Arch/Ubuntu)

local function escape(str)
	local escape_chars = [[;,."|\]]
	return vim.fn.escape(str, escape_chars)
end

local en_shift = [[~QWERTYUIOP{}ASDFGHJKL:"ZXCVBNM<>]]
local ru_shift = [[ËЙЦУКЕНГШЩЗХЪФЫВАПРОЛДЖЭЯЧСМИТЬБЮ]]
local en = [[`qwertyuiop[]asdfghjkl;'zxcvbnm]]
local ru = [[ёйцукенгшщзхъфывапролджэячсмить]]

vim.opt.langmap = vim.fn.join({
	escape(ru_shift) .. ";" .. escape(en_shift),
	escape(ru) .. ";" .. escape(en),
}, ",")

-- ===========================
--         АВТОУСТАНОВКА LAZY.NVIM
-- ===========================
local lazypath = vim.fn.stdpath("data") .. "/lazy/lazy.nvim"
if not vim.loop.fs_stat(lazypath) then
	print("Устанавливаю lazy.nvim...")
	vim.fn.system({ "git", "clone", "--filter=blob:none", "https://github.com/folke/lazy.nvim.git", lazypath })
end
vim.opt.rtp:prepend(lazypath)

-- ===========================
--         НАСТРОЙКА ПЛАГИНОВ
-- ===========================
require("lazy").setup({

	-- ======== ТЕМЫ ========
	{ "folke/tokyonight.nvim", priority = 900 },
	{ "olimorris/onedarkpro.nvim", priority = 1000 },

	-- В lazy.nvim
	{
		"williamboman/mason.nvim",
		config = function()
			require("mason").setup()
		end,
	},

	{
		"WhoIsSethDaniel/mason-tool-installer.nvim",
		dependencies = { "williamboman/mason.nvim" },
	},

	-- ======== TELESCOPE ========
	{
		"nvim-telescope/telescope.nvim",
		tag = "0.1.8",
		dependencies = { "nvim-lua/plenary.nvim" },
		config = function()
			local builtin = require("telescope.builtin")
			vim.keymap.set("n", "<leader>ff", function()
				builtin.find_files({
					layout_strategy = "vertical",
					layout_config = { prompt_position = "top" },
				})
			end)
		end,
	},

	{
		"chomosuke/typst-preview.nvim",
		lazy = false, -- or ft = 'typst'
		version = "1.*",
		opts = {}, -- lazy.nvim will implicitly calls `setup {}`
	},

	-- ======== КОММЕНТАРИИ ========
	{
		"numToStr/Comment.nvim",
		opts = {
			-- Конфигурация по умолчанию
			padding = true, -- Добавлять пробел после комментария
			sticky = true, -- Курсор остается на месте после комментирования
			ignore = nil, -- Регулярные выражения для игнорирования
			toggler = {
				line = "gcc", -- Комментирование одной строки
				block = "gbc", -- Комментирование блока
			},
			opleader = {
				line = "gc", -- Комментирование в визуальном режиме
				block = "gb", -- Комментирование блока в визуальном режиме
			},
			extra = {
				above = "gcO", -- Добавить комментарий выше
				below = "gco", -- Добавить комментарий нижеrmehri01/onenord.nvim
				eol = "gcA", -- Добавить комментарий в конец строки
			},
			mappings = {
				basic = true, -- Включить базовые маппинги
				extra = true, -- Включить дополнительные маппинги
			},
		},
		config = function()
			require("Comment").setup()

			-- Кастомные маппинги для Space + /
			vim.keymap.set("n", "<leader>/", function()
				require("Comment.api").toggle.linewise.current()
			end, { desc = "Комментировать строку" })

			vim.keymap.set("v", "<leader>/", function()
				local esc = vim.api.nvim_replace_termcodes("<ESC>", true, false, true)
				vim.api.nvim_feedkeys(esc, "nx", false)
				require("Comment.api").toggle.linewise(vim.fn.visualmode())
			end, { desc = "Комментировать выделение" })
		end,
	},

	-- ======== ФАЙЛОВЫЙ БРАУЗЕР ========
	{
		"nvim-tree/nvim-tree.lua",
		dependencies = { "nvim-tree/nvim-web-devicons" },
		config = function()
			require("nvim-tree").setup({
				view = { width = 35 },
				renderer = { highlight_git = true },
				actions = { open_file = { quit_on_open = false } },
			})
			vim.keymap.set("n", "<C-n>", ":NvimTreeToggle<CR>", { noremap = true, silent = true })
		end,
	},

	-- ======== ПОДСВЕТКА ========
	{
		"nvim-telescope/telescope.nvim",
		tag = "0.1.8",
		dependencies = { "nvim-lua/plenary.nvim" },
		config = function()
			local builtin = require("telescope.builtin")
			vim.keymap.set("n", "<leader>ff", function()
				builtin.find_files({
					-- ===========================
					layout_strategy = "horizontal", -- 'horizontal' → список слева, предпросмотр справа
					-- 'vertical' → список сверху, предпросмотр снизу
					layout_config = {
						prompt_position = "top", -- строка поиска вверху
						preview_width = 0.5, -- сколько места занимает предпросмотр справа (0.0 - 1.0)
						width = 0.9, -- ширина всего окна Telescope (0.0 - 1.0)
						height = 0.8, -- высота окна (0.0 - 1.0)
						mirror = false, -- если true, меняет местами список и превью
					},
					sorting_strategy = "ascending", -- 'ascending' → список сверху вниз (Tab вниз)
					-- 'descending' → список снизу вверх
					-- ===========================
				})
			end)
		end,
	},

	-- ======== АВТОДОПОЛНЕНИЕ ========
	{
		"hrsh7th/nvim-cmp",
		dependencies = {
			"hrsh7th/cmp-nvim-lsp",
			"hrsh7th/cmp-buffer",
			"hrsh7th/cmp-path",
			"L3MON4D3/LuaSnip",
			"saadparwaiz1/cmp_luasnip",
			"hrsh7th/cmp-nvim-lsp-signature-help",
		},
		config = function()
			local cmp = require("cmp")
			local luasnip = require("luasnip")

			cmp.setup({
				snippet = {
					expand = function(args)
						luasnip.lsp_expand(args.body)
					end,
				},
				mapping = cmp.mapping.preset.insert({
					["<CR>"] = cmp.mapping.confirm({ select = true }),
					["<Tab>"] = cmp.mapping(function(fallback)
						local col = vim.fn.col(".") - 1
						if cmp.visible() then
							cmp.select_next_item()
						elseif luasnip.expand_or_jumpable() then
							luasnip.expand_or_jump()
						elseif col == 0 or vim.fn.getline("."):sub(col, col):match("%s") then
							-- Если в начале строки или перед курсором пробел, вставляем таб
							vim.api.nvim_feedkeys(vim.api.nvim_replace_termcodes("<Tab>", true, false, true), "n", true)
						else
							fallback()
						end
					end, { "i", "s" }),

					["<S-Tab>"] = cmp.mapping(function(fallback)
						if cmp.visible() then
							cmp.select_prev_item()
						elseif luasnip.jumpable(-1) then
							luasnip.jump(-1)
						else
							fallback()
						end
					end, { "i", "s" }),
				}),
				sources = cmp.config.sources({
					{ name = "nvim_lsp" },
					{ name = "nvim_lsp_signature_help" },
					{ name = "luasnip" },
					{ name = "buffer" },
					{ name = "path" },
				}),
				-- ДОБАВЬТЕ ЭТОТ БЛОК ДЛЯ СТИЛЕЙ ОКОН:
				window = {
					completion = {
						border = "rounded",
						winhighlight = "Normal:NormalFloat,FloatBorder:FloatBorder,Search:None",
						scrollbar = true,
					},
					documentation = {
						border = "rounded",
						winhighlight = "Normal:NormalFloat,FloatBorder:FloatBorder,Search:None",
						scrollbar = true,
						max_width = 80,
						max_height = 20,
					},
				},
				formatting = {
					fields = { "kind", "abbr", "menu" },
					format = function(entry, vim_item)
						local kind_icons = {
							Text = "",
							Method = "󰆧",
							Function = "󰊕",
							Constructor = "",
							Field = "󰇽",
							Variable = "󰂡",
							Class = "󰠱",
							Interface = "",
							Module = "",
							Property = "󰜢",
							Unit = "",
							Value = "󰎠",
							Enum = "",
							Keyword = "󰌋",
							Snippet = "",
							Color = "󰏘",
							File = "󰈙",
							Reference = "",
							Folder = "󰉋",
							EnumMember = "",
							Constant = "󰏿",
							Struct = "",
							Event = "",
							Operator = "󰆕",
							TypeParameter = "󰅲",
						}

						vim_item.kind = string.format("%s %s", kind_icons[vim_item.kind], vim_item.kind)
						vim_item.menu = ({
							nvim_lsp = "[LSP]",
							luasnip = "[Snippet]",
							buffer = "[Buffer]",
							path = "[Path]",
						})[entry.source.name]

						return vim_item
					end,
				},
				view = {
					entries = { name = "custom", selection_order = "near_cursor" },
				},
				preselect = cmp.PreselectMode.Item,
			})

			vim.keymap.set("i", "<Esc>", function()
				if cmp.visible() then
					cmp.close() -- закрываем автоподсказки
				else
					-- если меню не открыто, просто делаем стандартный Esc
					vim.api.nvim_feedkeys(vim.api.nvim_replace_termcodes("<Esc>", true, false, true), "n", true)
				end
			end, { noremap = true, silent = true })
		end,
	},

	-- ======== LSP (СМАРТ-ПРОЕКТЫ) ========
	{
		"neovim/nvim-lspconfig",
		dependencies = {
			"williamboman/mason.nvim",
			"williamboman/mason-lspconfig.nvim",
			"hrsh7th/cmp-nvim-lsp",
		},
		config = function()
			-- Настройка Mason
			require("mason").setup()
			require("mason-lspconfig").setup({
				ensure_installed = {
					"zls",
					"tinymist",
					"pyright",
					"clangd",
					"lua_ls",
					"ruff",
				},
				automatic_installation = true,
			})
			require("mason-tool-installer").setup({
				ensure_installed = {
					"stylua",
					"clang-format",
				},
				run_on_start = true,
			})

			local capabilities = require("cmp_nvim_lsp").default_capabilities()

			-- Настройка LSP серверов через новый API vim.lsp.start
			local servers = {

				clangd = {
					capabilities = capabilities,
					cmd = {
						"clangd",
						"--inlay-hints=true",
						"--inlay-hints-parameter-names=true",
						"--inlay-hints-deduced-types=true",
					},
				},

				pyright = {
					capabilities = capabilities,
				},
				lua_ls = {
					cmd = { "lua-language-server" },
					capabilities = capabilities,
					settings = {
						Lua = {
							diagnostics = { globals = { "vim" } },
							workspace = { checkThirdParty = false },
						},
					},
				},

				zls = {
					capabilities = capabilities,
					settings = {
						zls = {
							enable_inlay_hints = true,
							inlay_hints_show_parameter_name = true,
							inlay_hints_show_variable_type = true,
						},
					},
				},
				tinymist = {
					capabilities = capabilities,
				},

				ruff = {
					capabilities = capabilities,
				},
			}

			-- Включаем Inlay Hints если сервер поддерживает
			vim.api.nvim_create_autocmd("LspAttach", {
				callback = function(args)
					local client = vim.lsp.get_client_by_id(args.data.client_id)
					local bufnr = args.buf

					if client and client.server_capabilities.inlayHintProvider then
						vim.lsp.inlay_hint.enable(true, { bufnr = bufnr })
					end
				end,
			})

			-- Автоматический запуск LSP серверов при открытии файлов
			vim.api.nvim_create_autocmd("FileType", {
				pattern = "*",
				callback = function(args)
					local bufnr = args.buf
					local ft = vim.bo[bufnr].filetype

					-- Соответствие типов файлов и LSP серверов

					local servers_by_ft = {
						c = "clangd",
						cpp = "clangd",
						python = "pyright",
						lua = "lua_ls",
						zig = "zls",
						typst = "tinymist",
					}

					local server_name = servers_by_ft[ft]
					if server_name and servers[server_name] then
						-- Используем новый API vim.lsp.start
						vim.lsp.start({
							name = server_name,
							bufnr = bufnr,
							capabilities = servers[server_name].capabilities,
							settings = servers[server_name].settings,
							root_dir = vim.fs.dirname(vim.fs.find({
								"CMakeLists.txt",
								"compile_commands.json",
								"pyproject.toml",
								"setup.py",
								"requirements.txt",
								"build.zig",
								".git",
							}, { upward = true })[1] or vim.loop.cwd()),
						})
					end
				end,
			})

			-- Ручной запуск LSP для уже открытых буферов
			vim.api.nvim_create_autocmd("VimEnter", {
				callback = function()
					for _, bufnr in ipairs(vim.api.nvim_list_bufs()) do
						if vim.api.nvim_buf_is_loaded(bufnr) then
							local ft = vim.api.nvim_buf_get_option(bufnr, "filetype")

							local servers_by_ft = {
								c = "clangd",
								cpp = "clangd",
								python = "pyright",
								lua = "lua_ls",
								zig = "zls",
								typst = "tinymist",
							}

							local server_name = servers_by_ft[ft]
							if server_name and servers[server_name] then
								vim.lsp.start({
									name = server_name,
									bufnr = bufnr,
									capabilities = servers[server_name].capabilities,
									settings = servers[server_name].settings,
									root_dir = vim.fs.dirname(vim.fs.find({
										"CMakeLists.txt",
										"compile_commands.json",
										"pyproject.toml",
										"setup.py",
										"requirements.txt",
										"build.zig",
										".git",
									}, { upward = true })[1] or vim.loop.cwd()),
								})
							end
						end
					end
				end,
			})

			-- Клавиши для LSP
			vim.keymap.set("n", "gd", vim.lsp.buf.definition, { desc = "Перейти к определению" })
			vim.keymap.set("n", "gr", vim.lsp.buf.references, { desc = "Найти ссылки" })
			vim.keymap.set("n", "K", vim.lsp.buf.hover, { desc = "Показать документацию" })
			vim.keymap.set("n", "<leader>rn", vim.lsp.buf.rename, { desc = "Переименовать" })
			vim.keymap.set("n", "<leader>ca", vim.lsp.buf.code_action, { desc = "Код действия" })

			-- Ошибки только при наведении
			vim.diagnostic.config({
				virtual_text = false,
				underline = true,
				signs = false,
				update_in_insert = true,
			})

			vim.api.nvim_create_autocmd("CursorHold", {
				callback = function()
					vim.diagnostic.open_float(nil, {
						focusable = false,
						border = "rounded",
						source = "always",
						prefix = " ",
					})
				end,
			})
		end,
	},

	-- ======== ФОРМАТИРОВАНИЕ ========
	{
		"stevearc/conform.nvim",
		config = function()
			require("conform").setup({
				formatters_by_ft = {
					c = { "clang-format" },
					cpp = { "clang-format" },
					python = { "ruff_format" },
					lua = { "stylua" },
					zig = { "zig fmt" },
				},
				format_on_save = { timeout_ms = 500, lsp_fallback = true },
			})
		end,
	},

	-- ======== ВКЛАДКИ ========
	{
		"akinsho/bufferline.nvim",
		version = "*",
		dependencies = "nvim-tree/nvim-web-devicons",
		config = function()
			require("bufferline").setup({})
			vim.keymap.set("n", "<Tab>", ":BufferLineCycleNext<CR>", { noremap = true, silent = true })
			vim.keymap.set("n", "<S-Tab>", ":BufferLineCyclePrev<CR>", { noremap = true, silent = true })
			vim.keymap.set(
				"n",
				"<leader>x",
				":bd<CR>",
				{ noremap = true, silent = true, desc = "Закрыть вкладку" }
			)
		end,
	},

	-- ======== АВТОСКОБКИ ========
	{ "windwp/nvim-autopairs", event = "InsertEnter", config = true },

	-- ======== СТАТУСБАР ========
	{
		"nvim-lualine/lualine.nvim",
		dependencies = { "nvim-tree/nvim-web-devicons" },
		config = function()
			require("lualine").setup({
				options = {
					theme = "tokyonight",
					section_separators = "",
					component_separators = "",
				},
				sections = {
					lualine_a = { "mode" },
					lualine_b = { "branch", "diff" },
					lualine_c = { "filename" },
					lualine_x = { "diagnostics", "encoding", "fileformat", "filetype" },
					lualine_y = { "progress" },
					lualine_z = { "location" },
				},
			})
		end,
	},
	-- ======== ТЕРМИНАЛ ========
	{
		"akinsho/toggleterm.nvim",
		version = "*",
		config = function()
			require("toggleterm").setup({
				open_mapping = [[<A-h>]],
				direction = "float",
				close_on_exit = true,
				persist_size = true,

				float_opts = {
					border = "rounded", -- single | double | shadow | rounded
					width = function()
						return math.floor(vim.o.columns * 0.85)
					end,
					height = function()
						return math.floor(vim.o.lines * 0.75)
					end,
					winblend = 0, -- если хочешь прозрачность: 5–15
				},
			})
		end,
	},

	{ "rafi/awesome-vim-colorschemes" },
})

vim.keymap.set("n", "<leader>tt", function()
	require("telescope.builtin").colorscheme({
		enable_preview = true,
	})
end, { desc = "Выбор темы (Telescope)" })

-- ===========================
--   СОХРАНЕНИЕ ЦВЕТОВОЙ ТЕМЫ
-- ===========================

local theme_file = vim.fn.stdpath("config") .. "/.last_colorscheme"

-- При смене темы — сохраняем
vim.api.nvim_create_autocmd("ColorScheme", {
	callback = function()
		local theme = vim.g.colors_name
		if theme then
			local f = io.open(theme_file, "w")
			if f then
				f:write(theme)
				f:close()
			end
		end
	end,
})

-- При старте — загружаем сохранённую тему
vim.api.nvim_create_autocmd("VimEnter", {
	once = true,
	callback = function()
		local f = io.open(theme_file, "r")
		if f then
			local theme = f:read("*l")
			f:close()
			if theme and theme ~= "" then
				pcall(vim.cmd, "colorscheme " .. theme)
			end
		end
	end,
})

-- ===========================
--   FLOATING TERMINAL STYLE
-- ===========================

local function style_terminal()
	-- фон терминала
	vim.api.nvim_set_hl(0, "ToggleTermNormal", {
		bg = "#0f1117", -- нейтральный тёмный
	})

	-- рамка
	vim.api.nvim_set_hl(0, "ToggleTermBorder", {
		fg = "#5e81ac", -- холодный акцент
	})

	-- на всякий случай
	vim.api.nvim_set_hl(0, "NormalFloat", {
		bg = "#0f1117",
	})
end

style_terminal()

-- если вдруг тема перезаписывает — вернём обратно
vim.api.nvim_create_autocmd("ColorScheme", {
	callback = style_terminal,
})

-- ===========================
--         ДОПОЛНИТЕЛЬНО
-- ===========================
-- Форматирование Ctrl + s
vim.keymap.set("n", "<C-s>", function()
	require("conform").format({ async = true })
end, { desc = "Форматировать код" })

vim.api.nvim_set_keymap("i", "jk", "<Esc>", { noremap = false })

-- Очищать подсветку поиска по нажатию Escape в нормальном режиме
vim.keymap.set("n", "<Esc>", function()
	if vim.fn.getreg("/") ~= "" then -- если есть активный поиск
		vim.cmd("nohlsearch") -- очищаем подсветку
	end
	return vim.api.nvim_replace_termcodes("<Esc>", true, false, true)
end, { noremap = true, expr = true })
