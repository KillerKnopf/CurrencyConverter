#include "Account.h"
#include <format>

namespace CurrencyConverter
{
	Quotas::Quotas()
	{
		this->total = 0;
		this->used = 0;
		this->remaining = 0;
	}

	Quotas::Quotas(uint32_t total, uint32_t used, uint32_t remaining)
	{
		this->total = total;
		this->used = used;
		this->remaining = remaining;
	}

	Quotas::~Quotas()
	{
	}

	string Quotas::to_string()
	{
		return std::format("\tTotal quotas: {}\n\tUsed quotas: {}\n\tRemaining quotas: {}\n", this->total, this->used, this->remaining);
	}

	Grace::Grace()
	{
		this->total = 0;
		this->used = 0;
		this->remaining = 0;
	}

	Grace::Grace(uint32_t total, uint32_t used, uint32_t remaining)
	{
		this->total = total;
		this->used = used;
		this->remaining = remaining;
	}

	Grace::~Grace()
	{
	}

	string Grace::to_string()
	{
		return std::format("\tTotal grace: {}\n\tUsed grace: {}\n\tRemaining grace: {}\n", this->total, this->used, this->remaining);
	}

	Account::Account()
	{
		this->account_id = "";
		this->quotas = Quotas();
		this->grace = Grace();
	}

	Account::Account(string account_id, uint32_t quotas_total, uint32_t quotas_used, uint32_t quotas_remaining, uint32_t grace_total, uint32_t grace_used, uint32_t grace_remaining)
	{
		this->account_id = account_id;
		this->quotas = Quotas(quotas_total, quotas_used, quotas_remaining);
		this->grace = Grace(grace_total, grace_used, grace_remaining);
	}

	Account::~Account()
	{
	}

	string Account::to_string()
	{
		return std::format("Account id: {}\n    Quotas\n{}    Grace\n{}", this->account_id, this->quotas.to_string(), this->grace.to_string());
	}
}
